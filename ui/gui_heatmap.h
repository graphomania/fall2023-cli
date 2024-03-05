#ifndef GUI_HEATMAP_H
#define GUI_HEATMAP_H

#include "../internal/function.h"

#include <QWidget>
#include <QPainter>

class HeatmapWidget final : public QWidget {
public:
    explicit HeatmapWidget(QWidget* parent = nullptr) : QWidget(parent) {}

    HeatmapWidget* rate(const size_t rate) {
        rate_ = rate;
        return this;
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int width = this->width();
        const int height = this->height();

        if (!draw_) {
            painter.fillRect(0, 0, width, height, QColor{255, 255, 255});
            return;
        }

        for (size_t x = 0; x < width; x += pixel_size_) {
            for (size_t y = 0; y < height; y += pixel_size_) {
                double sum = 0.0;
                int count = 0;

                for (size_t i = x; i < x + pixel_size_; i += rate_) {
                    for (size_t j = y; j < y + pixel_size_; j += rate_) {
                        double realX = /*1 - */experiment_.area->percentile(0, static_cast<double>(i) / width);
                        double realY = experiment_.area->percentile(1, static_cast<double>(j) / height);
                        sum += (*experiment_.function)({{realX, realY}});
                        count++;
                    }
                }

                const double averageResult = sum / count;
                const double temperature = std::min(map(averageResult, 0.0, 100.0, 0.0, 1.0), 0.9999);
                QColor color = temperatureToColor(temperature);
                painter.fillRect(x, height - y - 1, pixel_size_, pixel_size_, color);
            }
        }

        painter.setPen({97, 222, 42});
        for (size_t i = 1; i < experiment_.path.size(); i++) {
            const auto left = experiment_.area->point_to_percentile(experiment_.path[i - 1]);
            const auto right = experiment_.area->point_to_percentile(experiment_.path[i]);
            painter.drawLine(QPoint{
                                 static_cast<int>(left.first * width),
                                 height - 1 - static_cast<int>(left.second * height)
                             },
                             QPoint{
                                 static_cast<int>(right.first * width),
                                 height - 1 - static_cast<int>(right.second * height)
                             });
        }
    }

    HeatmapWidget* with(const Experiment& experiment) {
        experiment_ = experiment;
        return this;
    }

    HeatmapWidget* with(const size_t rate = 3, const size_t pixel_size = 8) {
        rate_ = rate;
        pixel_size_ = pixel_size;
        return this;
    }

    HeatmapWidget* with(const std::function<void(bool)>& callback) {
        callback_ = callback;
        return this;
    }

    HeatmapWidget* draw(const bool flag) {
        draw_ = flag;
        return this;
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && experiment_.area->dimensions() == 2) {
            const int width = this->width();
            const int height = this->height();
            double realX = experiment_.area->percentile(0, static_cast<double>(event->pos().x()) / width);
            double realY = experiment_.area->percentile(1, static_cast<double>(height - event->pos().y()) / height);

            // experiment_.method->with_start({{realX, realY}});
            start_ = {{{realX, realY}}};
            fmt::println("{} {} in {}", realX, realY, experiment_.area->to_string());
            callback_(true);
        }
    }

    [[nodiscard]]
    std::optional<Point> start() const {
        return start_;
    };

private:
    Experiment experiment_;
    std::function<void(bool)> callback_;
    std::optional<Point> start_;
    size_t rate_ = 3, pixel_size_ = 8;
    bool draw_ = true;

    static QColor temperatureToColor(double temperature) {
        int r = static_cast<int>(255 * pow(temperature, 3));
        int b = static_cast<int>(255 * pow(1 - temperature, 3));
        if (r > 255 || b > 255) {
            return {r, 0, 0};
        }
        return {r, 0, b};
    }

    static double map(double value, double start1, double end1, double start2, double end2) {
        return (start2 + (end2 - start2) * ((value - start1) / (end1 - start1)));
    }
};

#endif //GUI_HEATMAP_H
