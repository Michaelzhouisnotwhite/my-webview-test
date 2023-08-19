#include "webview.h"
#ifdef WIN32
#include <windows.h>
#include <dwmapi.h>
#endif
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include "project.h"


using namespace std::literals;
class Log {
public:
    std::ofstream log_file;
    Log() : log_file(LOG_FILE_PATH, std::ios::out) {
        if (!log_file.is_open()) {
            abort();
        }
    }
    void info(const std::string& log) {
        log_file.write(log.c_str(), (int64_t)log.length());
    }
    ~Log() {
        log_file.close();
    }
};

static Log logger;
constexpr const auto html =
    R"html(<button id="increment">Tap me</button>
<div>You tapped <span id="count">0</span> time(s).</div>
<button id="compute">Compute</button>
<div>Result of computation: <span id="compute-result">0</span></div>
<script>
  const [incrementElement, countElement, computeElement, computeResultElement] =
    document.querySelectorAll("#increment, #count, #compute, #compute-result");
  document.addEventListener("DOMContentLoaded", () => {
    incrementElement.addEventListener("click", () => {
      window.increment().then(result => {
        countElement.textContent = result.count;
      });
    });
    computeElement.addEventListener("click", () => {
      computeElement.disabled = true;
      window.compute(6, 7).then(result => {
        computeResultElement.textContent = result;
        computeElement.disabled = false;
      });
    });
  });
</script>)html";
#include "project.h"
auto get_html = []() {
    std::ifstream file(HTML_PATH, std::ios::in);
    if (!file.is_open()) {
        file.close();
        throw std::runtime_error("file not open");
    }
    std::string res;
    std::stringstream stream;
    while (std::getline(file, res)) {
        stream << res << "\n";
    }
    file.close();
#if DEBUG
    auto _str_res = stream.str();
    std::puts(_str_res.c_str());
    logger.info(_str_res);
#endif
    return stream.str();
};

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
#else
int main() {
#endif
    unsigned int count = 0;
    webview::webview w(true, nullptr);
    w.set_title("Bind Example");
    w.set_size(800, 800, WEBVIEW_HINT_MIN);
    // w.dispatch([&]() {

    // });
    // HWND hwind = (HWND)w.window();
    
    // HWND hWnd = (HWND)w.window();                                       // 获取主窗口句柄
    // DWORD dwAttribute = DWMWA_USE_IMMERSIVE_DARK_MODE;                  // 设置暗色模式属性
    // BOOL bValue = TRUE;                                                 // 启用暗色模式
    // DwmSetWindowAttribute(hWnd, dwAttribute, &bValue, sizeof(bValue));  // 设置窗口属性
    // A binding that increments a value and immediately returns the new value.
    w.bind("increment", [&](const std::string& /*req*/) -> std::string {
        auto count_string = std::to_string(++count);
        // std::this_thread::sleep_for(1s);
        return "{\"count\": " + count_string + "}";
    });

    // An binding that creates a new thread and returns the result at a later time.
    w.bind(
        "compute",
        [&](const std::string& seq, const std::string& req, void* /*arg*/) {
            // Create a thread and forget about it for the sake of simplicity.
            std::thread([&, seq, req] {
                // Simulate load.
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // json_parse() is an implementation detail and is only used here
                // to provide a working example.
                auto left = std::stoll(webview::detail::json_parse(req, "", 0));
                auto right = std::stoll(webview::detail::json_parse(req, "", 1));
                auto result = std::to_string(left * right);
                w.resolve(seq, 0, result);
            }).detach();
        },
        nullptr);

    // w.set_html(get_html());
    w.navigate("http://127.0.0.1:8080/");
    w.run();

    return 0;
}