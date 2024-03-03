#include "../ui/cli.h"
#include "../ui/gui.h"


int main(int argc, char* argv[]) {
    if (argc == 1) {
        QApplication app(argc, argv);
        return MainWindow(&app).start();
    }

    return cli_app(argc, argv);
}
