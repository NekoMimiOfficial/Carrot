#include "carrot_module.h"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMetaObject>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct QtSession {
  std::thread uiThread;
  std::atomic<bool> running{false};

  QApplication *app = nullptr;
  QMainWindow *mainWindow = nullptr;
  QVBoxLayout *mainLayout = nullptr;

  std::atomic<int> nextWidgetId{1};
  std::unordered_map<int, QWidget *> widgets;

  std::mutex mutex;
  std::condition_variable cv;

  explicit QtSession(const std::string &title, int width, int height) {
    running = true;

    uiThread = std::thread([this, title, width, height]() {
      int argc = 1;
      char arg0[] = "carrot_qt";
      char *argv[] = {arg0, nullptr};

      QApplication qapp(argc, argv);
      QMainWindow win;
      win.setWindowTitle(QString::fromStdString(title));
      win.resize(width, height);

      QWidget *central = new QWidget(&win);
      QVBoxLayout *layout = new QVBoxLayout(central);
      win.setCentralWidget(central);

      {
        std::lock_guard<std::mutex> lock(mutex);
        this->app = &qapp;
        this->mainWindow = &win;
        this->mainLayout = layout;
      }
      cv.notify_one();

      qapp.exec();

      {
        std::lock_guard<std::mutex> lock(mutex);
        this->app = nullptr;
        this->mainWindow = nullptr;
        this->mainLayout = nullptr;
        this->widgets.clear();
        this->running = false;
      }
    });

    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]() { return mainWindow != nullptr; });
  }

  ~QtSession() { close(); }

  void close() {
    if (running && app) {
      QMetaObject::invokeMethod(app, &QApplication::quit, Qt::QueuedConnection);
    }
    if (uiThread.joinable()) {
      uiThread.join();
    }
  }

  template <typename F> void runOnUI(F &&f) {
    if (!app)
      return;
    QMetaObject::invokeMethod(app, std::forward<F>(f), Qt::QueuedConnection);
  }
};

struct AddTextFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit AddTextFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 1; }
  std::string name() override { return "addText"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("addText requires a String argument.");

    std::string text = std::get<std::string>(args[0]);
    int id = session->nextWidgetId++;

    auto sess = session;
    sess->runOnUI([sess, id, text]() {
      if (!sess->mainLayout)
        return;
      QLabel *label = new QLabel(QString::fromStdString(text));
      sess->mainLayout->addWidget(label);
      sess->widgets[id] = label;
    });

    return static_cast<double>(id);
  }
};

struct AddImageFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit AddImageFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 1; }
  std::string name() override { return "addImage"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("addImage requires a String filepath argument.");

    std::string path = std::get<std::string>(args[0]);
    int id = session->nextWidgetId++;

    auto sess = session;
    sess->runOnUI([sess, id, path]() {
      if (!sess->mainLayout)
        return;
      QLabel *label = new QLabel();
      label->setPixmap(QPixmap(QString::fromStdString(path)));
      sess->mainLayout->addWidget(label);
      sess->widgets[id] = label;
    });

    return static_cast<double>(id);
  }
};

struct AddButtonFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit AddButtonFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 2; }
  std::string name() override { return "addButton"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]) ||
        !std::holds_alternative<std::shared_ptr<NinCallable>>(args[1])) {
      throw std::runtime_error(
          "addButton requires (String text, Callable onClick).");
    }

    std::string text = std::get<std::string>(args[0]);
    auto callback = std::get<std::shared_ptr<NinCallable>>(args[1]);
    int id = session->nextWidgetId++;

    auto sess = session;
    sess->runOnUI([sess, id, text, callback]() {
      if (!sess->mainLayout)
        return;
      QPushButton *btn = new QPushButton(QString::fromStdString(text));

      QObject::connect(btn, &QPushButton::clicked, [callback]() {
        try {
          callback->call({});
        } catch (const std::exception &e) {
          std::cerr << "[QtUI] Carrot Callback Error: " << e.what() << "\n";
        }
      });

      sess->mainLayout->addWidget(btn);
      sess->widgets[id] = btn;
    });

    return static_cast<double>(id);
  }
};

struct SetTextFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit SetTextFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 2; }
  std::string name() override { return "setText"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]) ||
        !std::holds_alternative<std::string>(args[1]))
      throw std::runtime_error("setText requires (Number id, String text).");

    int id = static_cast<int>(std::get<double>(args[0]));
    std::string text = std::get<std::string>(args[1]);

    auto sess = session;
    sess->runOnUI([sess, id, text]() {
      auto it = sess->widgets.find(id);
      if (it != sess->widgets.end()) {
        QWidget *w = it->second;
        if (QLabel *label = qobject_cast<QLabel *>(w)) {
          label->setText(QString::fromStdString(text));
        } else if (QPushButton *btn = qobject_cast<QPushButton *>(w)) {
          btn->setText(QString::fromStdString(text));
        }
      }
    });

    return std::monostate{};
  }
};

struct SetImageFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit SetImageFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 2; }
  std::string name() override { return "setImage"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]) ||
        !std::holds_alternative<std::string>(args[1]))
      throw std::runtime_error(
          "setImage requires (Number id, String filepath).");

    int id = static_cast<int>(std::get<double>(args[0]));
    std::string path = std::get<std::string>(args[1]);

    auto sess = session;
    sess->runOnUI([sess, id, path]() {
      auto it = sess->widgets.find(id);
      if (it != sess->widgets.end()) {
        if (QLabel *label = qobject_cast<QLabel *>(it->second)) {
          label->setPixmap(QPixmap(QString::fromStdString(path)));
        }
      }
    });

    return std::monostate{};
  }
};

struct RemoveWidgetFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit RemoveWidgetFn(std::shared_ptr<QtSession> s)
      : session(std::move(s)) {}
  int arity() override { return 1; }
  std::string name() override { return "removeWidget"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]))
      throw std::runtime_error("removeWidget requires (Number id).");

    int id = static_cast<int>(std::get<double>(args[0]));

    auto sess = session;
    sess->runOnUI([sess, id]() {
      auto it = sess->widgets.find(id);
      if (it != sess->widgets.end()) {
        QWidget *w = it->second;
        if (sess->mainLayout) {
          sess->mainLayout->removeWidget(w);
        }
        w->deleteLater();
        sess->widgets.erase(it);
      }
    });

    return std::monostate{};
  }
};

struct ShowFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit ShowFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 0; }
  std::string name() override { return "show"; }

  Value call(std::vector<Value>) override {
    auto sess = session;
    sess->runOnUI([sess]() {
      if (sess->mainWindow) {
        sess->mainWindow->show();
      }
    });
    return std::monostate{};
  }
};

struct CloseFn : NinCallable {
  std::shared_ptr<QtSession> session;
  explicit CloseFn(std::shared_ptr<QtSession> s) : session(std::move(s)) {}
  int arity() override { return 0; }
  std::string name() override { return "close"; }

  Value call(std::vector<Value>) override {
    session->close();
    return std::monostate{};
  }
};

struct CreateWindowFn : NinCallable {
  int arity() override { return 3; }
  std::string name() override { return "createWindow"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]) ||
        !std::holds_alternative<double>(args[1]) ||
        !std::holds_alternative<double>(args[2])) {
      throw std::runtime_error(
          "createWindow requires (String title, Number width, Number height).");
    }

    std::string title = std::get<std::string>(args[0]);
    int width = static_cast<int>(std::get<double>(args[1]));
    int height = static_cast<int>(std::get<double>(args[2]));

    auto session = std::make_shared<QtSession>(title, width, height);
    auto klass = std::make_shared<NinClass>("QtWindow");
    auto inst = std::make_shared<NinInstance>(klass);

    inst->fields["addText"] = std::make_shared<AddTextFn>(session);
    inst->fields["addImage"] = std::make_shared<AddImageFn>(session);
    inst->fields["addButton"] = std::make_shared<AddButtonFn>(session);
    inst->fields["setText"] = std::make_shared<SetTextFn>(session);
    inst->fields["setImage"] = std::make_shared<SetImageFn>(session);
    inst->fields["removeWidget"] = std::make_shared<RemoveWidgetFn>(session);
    inst->fields["show"] = std::make_shared<ShowFn>(session);
    inst->fields["close"] = std::make_shared<CloseFn>(session);

    return inst;
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["createWindow"] = std::make_shared<CreateWindowFn>();
}
