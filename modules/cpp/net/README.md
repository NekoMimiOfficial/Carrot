# Carrot, now with internet access!  
Gone are the days of darkness and here comes a new age of information all accessible via libcarrot_net!  
This new module aims to get you connected online for all your web request needs, anything from GET/POST to patching and deleting, we got it all  

# Features  
All features are documented in the `man.json` use the webdocs or doc renderer via `carrie` to view them in an organized way  

# Building  
This modules requires nothing but libcurl, install and build with cmake  

# Example  
```js
let net = loadmodule("@net");

fun str(obj) {
    return string.to_string(obj);
}

fun fetchUuid(url) {
    let response = net.get(url);
    response.raise_for_status();
    return response.text;
}

noot("doin ze GET");
let res = net.get("https://httpbin.org/get");

noot("Status Code: " + str(res.status_code));
noot("Request OK: " + str(res.ok));
noot("Elapsed Time (s): " + str(res.elapsed));
noot("Response Text:");
noot(res.text);

res.raise_for_status();

noot("doin ze POST");
let postRes = net.post(
    "https://httpbin.org/post",
    json= "{'message': 'Hello from Carrot!', 'active': true}"
);

noot("POST Status Code: " + str(postRes.status_code));
noot("POST Response Text:");
noot(postRes.text);

noot("doin ze Sesssion");
let session = net.Session();

let cookieSetRes = session.get("https://httpbin.org/cookies/set?user=carrot_dev");
noot("Set Cookie Status: " + str(cookieSetRes.status_code));

let cookieCheckRes = session.get("https://httpbin.org/cookies");
noot("Persisted Cookies Response:");
noot(cookieCheckRes.text);

noot("doin ze Func");
let uuidText = fetchUuid("https://httpbin.org/uuid");
noot("Fetched UUID Payload:");
noot(uuidText);

```

# Neko 200  
You've reached the endpoint, or have you... [:3c](https://github.com/NekoMimiOfficial)
