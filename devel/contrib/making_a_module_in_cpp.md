# CPP FFI modules  
In this section we will teach you all you need to know to start using the C++ bridge to add new features to Carrot the core interpreter can't support  
In short making modules means registering `Value`s into the Environment, the same way it works at runtime  
Think of it like this, you want to create a variable called "var" and assign the value "hello, world!" to it, you will tell the Environment to define `var` and set it's value to "hello, world!"  
As far as you'd be concerned the 3 main `Value` types you'd be using are `NinCallable` for functions and `NinClass` for classes and `NinNative` for custom types you create, you can of course add strings, bools, numbers and monostates but those are very self explanatory, just set the actual value as a string or double or bool etc for these types, as for the main 3 you'll need to `make_shared` to create a `shared_ptr` of them  
We will now talk about these types and how you can write them, these types are structs with different overrides to define data like the name, arity, callback etc  

## Value helpers  
Before we talk about types we will introduce some helpers that make development with types way easier (available from `methods.h` and `ctypeutils.h`):  
- **`getType`**: tells you what specific type a `Value` is  
- **`checkArgs`**: checks if an arg/kwarg is of a certain type, ex: `if (!checkArgs(args[0], "string")) { // throw an error }`  
- **`isInt`**: checks if a numeric value is an interger or a double  
- **`isTruthy`**: returns if value is true or false (or 0 or 1 or null)  
- **`isEqual`**: used to check if 2 `NinArray`s are the same  
- **`valueToString`**: converts any `Value` into a readable string  

You can manually check the type with `std::holds_alternative<Type>(Value)`  
You can save the type to a variable with `std::get<Type>(Value)`

## NinCallable  
The default function type in Carrot  
example:  
```cpp
struct TestModFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "init"; }
  bool isVariadic() override { return false; } // optional: defaults to false

  Value call(std::vector<Value>) override {
    return "hello, module!";
  }
};
```
In this example we can see a function defined as "TestModFn", this name doesn't affect the naming inside Carrot  
We set the arity which tells the interpreter how many args (not kwargs) this function expects, then we set the name, this is what the function will be called inside the interpreter, next we define if the function is variadic which tells the interpreter if the function expects kwargs  
from here we can define the callback (`call`) which takes a vec of `Value`s as an argument, these are your args, even if the arity is set to 0, in the callback you can do anything and then return any of the supported `Value`s by Carrot, even another `NinCallable` *or at least a `shared_ptr` of it*  
Seems pretty simple right?  
What if you wanna use kwargs?  
first lets imagine we set the arity to 0 and variadic to true, here we make the `call` return `callWithKwargs` which takes a vec of `Value`s and an unordered map of `<std::string, Value>` as kwargs  
Lets look at an example:  
```cpp
Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
}

Value callWithKwargs(std::vector<Value> args,
                     std::unordered_map<std::string, Value> kwargs) override {
    // do something here and return a Value
}
```
In this situation if you call the function with 0 args it will call the kwarg callback with the kwargs being empty  
Since the kwargs are an unordered_map you can iterate through them using something like:  
```cpp
auto it = kwargs.find("end");
if (it != kwargs.end()) {
  // do something with the kwarg
  kwargs.erase(it);
}
```
When dealing with args and kwargs you have to write your own safety checks for how many kwargs you got and the type of values they hold if it's what you expect otherwise you should throw a `runtime_error`, the convention is to start it with "functionName():" then whatever error message you want, due to this being dependant on the logic you want to write there's no specific way to handle the type checking, look at some examples for some insight.

## NinClass and NinInstance  
Your portal to grouping `Value`s together  
When using classes you need to instantiate them first before adding `Value`s to them, example:  
```cpp
auto klass = std::make_shared<NinClass>("className");
auto inst = std::make_shared<NinInstance>(klass);

inst->fields["init"] = std::make_shared<TestModFn>();
```
`inst` is the instance, and is what you'll be passing to the Environment, both types are `shared_ptr` of `NinClass` and `NinInstance` respectively  
fields hold `Value`s so you can place anything supported into there  
There's no point in using `NinClass` by itself for the user unless they want to specifically add callables and have the user instantiate the class before use, if so then use `klass->methods[] =`  
You can now access `className.init()` so for example if you have a utility for string manipulation you can add all the functions under a single instance.  

## NinNative  
Have you said there aren't enough types in Carrot?  
I hear ye, I hear ye...  
that's why we got you covered! :D  
let's take a lookie at how we can create new native types in Carrot:  
```cpp
// dont worry about this template, all this does is returns the value of a std::variant if it's the correct type else it returns a default value
// you can replace this with anything else that works or manually use getType or checkArgs to determine the type then use std::get
// in any case this is only needed because we need to make sure the setter gets the right type so so long as that is fulfilled any solution goes
template <typename T, typename... Types>
T get(const std::variant<Types...>& v, const T& default_value = T{}) {
    if (auto* p = std::get_if<T>(&v)) {
        return *p;
    }
    return default_value;
}

struct customType {
    double x;
    bool y;
    std::string z;
};

auto v = std::make_shared<customType>(); // define shared_ptr of custom type

auto native = std::make_shared<NinNative>();
native->typeName = "nativeType"; // this is what the type will be identified as
native->data = v; // data is a shared_ptr<void> so it holds any value so long as it's a shared_ptr

native->getField = [v](const std::string &f) -> Value {
    if (f == "x") return v->x;
    if (f == "y") return v->y;
    if (f == "z") return v->z;
    if (f == "someExtraThing")
        return (y)? x : z;
    throw std::runtime_error("customType has no field '" + f + "'.");
};

native->setField = [v](const std::string &f, Value val) {
    if (f == "x") v->x = get<double>(val, 0);
    else if (f == "y") v->y = get<bool>(val, false);
    else if (f == "z") v->z = get<std::string>(val, "defualt");
    else throw std::runtime_error("customType has no settable field '" + f + "'.");
};
```

Let me unpack what you just had to read, first i made a template to make getting values easier, this is no required but is nice to have, it gets the value of our `Value` based on what type we expect and if it's the wrong type it returns a default value  
then we made a struct just to define a clean structure to hold our data, we then need to make it shared_ptr  
now we define our shared_ptr of NinNative, from here we can start configuring it  
typeName will set the name that Carrot will use for this type (like string for example)  
then we give it a place to hold it's data, in this case the shared_ptr of our struct  
next are the setter and getter, the getter is a function that accepts the name of the requested field and in it you can do anything, including getting the values of x, y, z from our struct or making a field that for example returns the output of an equation on these data types, *or you can make a getter field that runs DOOM... :3c*  
the setter works in a similar way, it takes the name of the field you are setting and the value to set, here we will use the get template we created to make sure the type the user inputted matches the field type, of course you can also make custom fields to set like a field called yz which will for example set z if x is true, there's no limits, like seriously you can play DOOM so there's no limits XD  
and now you are capable of adding any type you want :D  

# Adding the types to the Environment  
Okay you made the types but what else? how do you get them into Carrot?  
You need to build a module with a C symbol called `carrot_module_init`  
How to? fine let's go step by step!  
first let's prep our directory, you'll need `carrot_module.h`, `CMakeLists.txt` and `mod.cpp`, you can add any other files as needed but these 3 are the main files you'll need, plus if you wish to publish you'll need to write a manual (`man.json`) and manifest (`manifest.json`) alongside a readme and liscense  
for now let's copy the `carrot_module.h` and `CMakeLists.txt` from the hello world example folder into our working folder and create an empty `mod.cpp` file  
you'll need to open the `CMakeLists.txt` file to change the project name, just make sure it's called `carrot_SOMETHING`, the "carrot_" is per convention  
you can also add any dependencies you'd need in the cmake file  
now we can start editing our `mod.cpp` file, include `carrot_module.h` and any necessary libraries then write your functions or classes/instances or natives, once you've got all the types you want to add define this:  
```cpp
extern "C" void carrot_module_init(std::unordered_map<std::string, Value> *out) {}
```

in here we can register our `Value`s like so `(*out)["init"] = std::make_shared<TestModFn>();` this is for the `NinCallable` example, you can do `...["something"] = std::make_shared<customType>()` and you can access your custom type named "something" in Carrot now  
Technically you don't have to use same name of your `NinCallable` in the Environment definition, it will always be known by what you define it as, we have `registerBuiltinFn` which registers the function by it's internal name but this is a helper for core development ie: builtin values and builtin platform values  
