#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H


#include "../internal/function.h"
#include "../internal/method.h"

#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QBoxLayout>


class CoordinateWidget final : public QWidget {
public:
    explicit CoordinateWidget(const QString& labelText, const QString& minText, const QString& maxText,
                              QWidget* parent = nullptr) : QWidget(parent) {
        const auto label = new QLabel(labelText, parent);
        min = new QLineEdit(minText, parent);
        max = new QLineEdit(maxText, parent);

        auto* layout = new QHBoxLayout(parent);
        layout->addWidget(label);
        layout->addWidget(min);
        layout->addWidget(max);

        setLayout(layout);
    }

    QLineEdit* min;
    QLineEdit* max;

    static void to(const QString& labelText, const QString& minText, const QString& maxText,
                   QVBoxLayout* addTo, std::vector<CoordinateWidget*>& pushTo,
                   QWidget* parent = nullptr) {
        const auto coord = new CoordinateWidget(labelText, minText, maxText, parent);
        addTo->addWidget(coord);
        pushTo.push_back(coord);
    }
};

struct Experiment {
    std::shared_ptr<Method> method;
    std::shared_ptr<Area> area;
    std::shared_ptr<Function> function;
    std::vector<Point> path;
};

struct GraphConfiguration {
    size_t pixelSize;
    size_t sampleDensity;
    bool flagDraw;
    int randomSeed;
};

class QLineEditWithLabel final : public QWidget {
public:
    explicit QLineEditWithLabel(const QString& labelText, const QString& defaultText,
                                QWidget* parent = nullptr) : QWidget(parent) {
        label = new QLabel(labelText, this);
        edit = new QLineEdit(defaultText, this);

        auto* layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(edit);
        setLayout(layout);
    }

    QLabel* label;
    QLineEdit* edit;
};


#endif //GUI_WIDGETS_H
