#include "../ui/cli.h"
#include "../ui/gui.h"


int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            QApplication app(argc, argv);
            return MainWindow(&app).start();
        }

        return cli_app(argc, argv);
    } catch (std::exception& e) {
        fmt::println(stderr, "RUNTIME ERROR: {}", e.what());
        return 1;
    } catch (...) {
        fmt::println(stderr, "WE ARE ON FIRE (probly), sorry about that");
        return -1;
    }
}
