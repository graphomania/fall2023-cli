#ifndef GUI_H
#define GUI_H

#include "../internal/function.h"
#include "gui_settings.h"

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QRadioButton>
#include <set>

#include <fmt/format.h>

#include "gui_heatmap.h"
#include "../internal/random.h"

class MainWindow final : public QMainWindow {
    QApplication* application_;
    HeatmapWidget* heatmap_widget_;
    QLabel* info_;
    SettingsDialog settingsDialog_;

public:
    explicit MainWindow(QApplication* wraps)
        : application_(wraps), heatmap_widget_(new HeatmapWidget(this)), info_(new QLabel("example", this)) {
        setMinimumSize(400, 300);
        setWindowTitle("Function Minima");
        resize(800, 600);
        graph_update(false);
    }

    void graph_update(bool save_start) const {
        try {
            heatmap_widget_->setGeometry(0, 30, width(), height() - 100);
            auto experiment = settingsDialog_.getExperiment();
            const auto [pixelSize, sampleDensity, drawGraph, seed] = settingsDialog_.getGraphConfiguration();
            random::engine() = std::mt19937_64(seed);
            heatmap_widget_->draw(drawGraph);

            if (save_start) {
                if (const auto start = heatmap_widget_->start(); start.has_value()) {
                    experiment.method->with_start(start.value());
                }
            }
            auto [path, mimima] = experiment.method->minimal_with_path(&*experiment.function, *experiment.area);
            experiment.path = path;

            heatmap_widget_->
                with(experiment)->
                with(sampleDensity, pixelSize)->
                with([this](bool flag) {
                    this->graph_update(flag);
                });
            heatmap_widget_->update();

            info_->setGeometry(20, height() - 80, width() - 20, 80);
            const auto text = fmt::format(">> {}. {}.\n"
                                          "Mimima: {}. Drawn path len: {}.\n"
                                          "Closest known minima: {}, MSE: {}.",
                                          experiment.function->name(), experiment.method->name(),
                                          mimima.first, path.size(),
                                          experiment.function->closest_minimal(mimima.first).first,
                                          experiment.function->closest_minimal(mimima.first).first.dist(mimima.first)
            );
            info_->setText(QString::fromStdString(text));
        } catch (const std::exception& e) {
            QMessageBox::critical(heatmap_widget_, "Unexpected error", e.what());
        }
    }

    void resizeEvent(QResizeEvent* event) override {
        graph_update(true);
    }

    int start() {
        QPushButton settingsButton("Settings", this);
        settingsButton.setGeometry(0, 0, 80, 30);
        QObject::connect(&settingsButton, &QPushButton::clicked, [this]() {
            if (settingsDialog_.exec() == QDialog::Accepted) {
                graph_update(false);
            }
        });

        show();
        return application_->exec();
    }
};

#endif //GUI_H
