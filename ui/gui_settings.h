#ifndef GUI_SETTINGS_H
#define GUI_SETTINGS_H


#include <QtWidgets>
#include <QRadioButton>

#include "../internal/method.h"
#include "../internal/method_nelder_mead.h"
#include "../internal/method_random_walk.h"

#include "parse.h"
#include "gui_widgets.h"


class SettingsDialog final : public QDialog {
public:
    explicit SettingsDialog(QWidget* parent = nullptr) : QDialog(parent) {
        const auto grid = new QGridLayout;
        grid->addWidget(createGroupMethod(), 0, 0);
        grid->addWidget(createGraphConfiguration(), 1, 0);
        grid->addWidget(createGroupFunction(), 0, 1);
        grid->addWidget(createOther(), 1, 1);
        okButton = new QPushButton("OK", this);
        grid->addWidget(okButton, 2, 1);
        connect(okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
        setLayout(grid);

        setWindowTitle(tr("Group Boxes"));
        resize(600, 480);
    }

    [[nodiscard]]
    GraphConfiguration getGraphConfiguration() const {
        return {
            static_cast<size_t>(must_int64(drawPixelSize->edit->text().toStdString(), true)),
            static_cast<size_t>(must_int64(drawGraphDensity->edit->text().toStdString(), true)),
            flagDrawGraph->isChecked(),
            static_cast<int>(must_int64(otherSeed->edit->text().toStdString(), false)),
        };
    }

    [[nodiscard]]
    Experiment getExperiment() const {
        std::shared_ptr<Method> method;
        std::shared_ptr<Function> function;
        std::shared_ptr<Area> area;

        auto logger = Log(Log::LEVEL::MUTED);
        switch (methodStackedWidget->currentIndex()) {
        case 0:
            method = std::make_shared<NelderMead>(NelderMead(
                    logger,
                    must_double(nelderMeadStopEps->text().toStdString(), positive<double>),
                    {},
                    must_double(nelderMeadAlpha->text().toStdString(), positive<double>),
                    must_double(nelderMeadGamme->text().toStdString(), positive<double>),
                    must_double(nelderMeadRho->text().toStdString(), positive<double>),
                    must_double(nelderMeadSigma->text().toStdString(), positive<double>)
                )
            );
            break;

        case 1:
            method = std::make_shared<RandomWalk>(
                logger,
                must_double(randomWalkDelta->text().toStdString(), positive<double>),
                must_double(randomWalkP->text().toStdString(), [](auto val) { return val >= 0 and val <= 1; }),
                must_double(randomWalkStopEps->text().toStdString(), positive<double>),
                must_int64(randomWalkMinSteps->text().toStdString(), false),
                must_int64(randomWalkSteps->text().toStdString(), false)
            );
            break;

        default:
            throw std::logic_error("?!");
        }

        auto parseArea = [&area](const std::vector<CoordinateWidget*>& areaWidget) {
            Point min, max;
            for (const auto& point : areaWidget) {
                min.push_back(must_double(point->min->text().toStdString()));
                max.push_back(must_double(point->max->text().toStdString()));
            }
            area = std::make_shared<Area>(Area{min, max});
        };

        switch (functionStackedWidget->currentIndex()) {
        case 0:
            function = std::make_shared<HimmelblauFunction>(HimmelblauFunction{});
            parseArea(functionHimmelblauArgArea);
            break;

        case 1:
            function = std::make_shared<StyblinskiTangFunction>(StyblinskiTangFunction{});
            parseArea(functionAckeyArgArea);
            break;

        case 2:
            function = std::make_shared<RastriginFunction>(RastriginFunction{2});
            parseArea(functionRastrigin2ArgArea);
            break;

        case 3:
            function = std::make_shared<RastriginFunction>(RastriginFunction{3});
            parseArea(functionRastrigin3ArgArea);
            break;

        case 4:
            function = std::make_shared<RastriginFunction>(RastriginFunction{4});
            parseArea(functionRastrigin4ArgArea);
            break;

        default:
            throw std::logic_error("?!");
        }

        if (area->dimensions() != 2) {
            flagDrawGraph->setChecked(false);
        }

        return Experiment{method, area, function};
    }

private:
    QRadioButton* rectangleButton;
    QRadioButton* circleButton;
    QPushButton* okButton;
    QLineEdit* nelderMeadStopEps;
    QLineEdit* nelderMeadAlpha;
    QWidget* pageNelderMead;
    QStackedWidget* methodStackedWidget;
    QLineEdit* nelderMeadGamme;
    QLineEdit* nelderMeadRho;
    QLineEdit* nelderMeadSigma;
    QWidget* pageRandomWalk;
    QLineEdit* randomWalkStopEps;
    QLineEdit* randomWalkSteps;
    QLineEdit* randomWalkMinSteps;
    QLineEdit* randomWalkP;
    QLineEdit* randomWalkDelta;
    QVBoxLayout* pageNelderMeadLayout;
    QVBoxLayout* pageRandomWalkLayout;
    QWidget* pageFunctionHimmelblau;
    QStackedWidget* functionStackedWidget;
    QVBoxLayout* pageFunctionHimmelblauLayout;
    std::vector<CoordinateWidget*> functionHimmelblauArgArea;
    std::vector<CoordinateWidget*> functionAckeyArgArea;
    std::vector<CoordinateWidget*> functionRastrigin3ArgArea;
    QVBoxLayout* pageFunctionRastriginLayout4;
    QWidget* pageFunctionRastrigin2;
    QWidget* pageFunctionRastrigin3;
    QWidget* pageFunctionRastrigin4;
    QVBoxLayout* pageFunctionRastriginLayout2;
    std::vector<CoordinateWidget*> functionRastrigin2ArgArea;
    QVBoxLayout* pageFunctionRastriginLayout3;
    std::vector<CoordinateWidget*> functionRastrigin4ArgArea;
    QGroupBox* flagDrawGraph;
    QLineEditWithLabel* drawGraphDensity;
    QLineEditWithLabel* drawPixelSize;


    QGroupBox* createGroupMethod() {
        const auto groupBox = new QGroupBox(tr("Modeling Methods"));
        const auto vboxLayout = new QVBoxLayout(this);
        groupBox->setLayout(vboxLayout);

        auto* comboBox = new QComboBox(this);
        comboBox->addItem(tr("Nelder-Mead"));
        comboBox->addItem(tr("Random Walk"));
        vboxLayout->addWidget(comboBox);

        methodStackedWidget = new QStackedWidget(this);
        vboxLayout->addWidget(methodStackedWidget);


        // Page 1, Nelder-Mead
        pageNelderMead = new QWidget(this);
        methodStackedWidget->addWidget(pageNelderMead);
        pageNelderMeadLayout = new QVBoxLayout(pageNelderMead);

        pageNelderMeadLayout->addWidget(new QLabel("Stop Eps."));
        nelderMeadStopEps = new QLineEdit(this);
        nelderMeadStopEps->setText("0.001");
        pageNelderMeadLayout->addWidget(nelderMeadStopEps);

        pageNelderMeadLayout->addWidget(new QLabel("Alpha"));
        nelderMeadAlpha = new QLineEdit(this);
        nelderMeadAlpha->setText("1");
        pageNelderMeadLayout->addWidget(nelderMeadAlpha);

        pageNelderMeadLayout->addWidget(new QLabel("Gamma"));
        nelderMeadGamme = new QLineEdit(this);
        nelderMeadGamme->setText("2");
        pageNelderMeadLayout->addWidget(nelderMeadGamme);

        pageNelderMeadLayout->addWidget(new QLabel("Rho"));
        nelderMeadRho = new QLineEdit(this);
        nelderMeadRho->setText("0.5");
        pageNelderMeadLayout->addWidget(nelderMeadRho);

        pageNelderMeadLayout->addWidget(new QLabel("Sigma"));
        nelderMeadSigma = new QLineEdit(this);
        nelderMeadSigma->setText("0.5");
        pageNelderMeadLayout->addWidget(nelderMeadSigma);


        // Page 2, Random Walk
        pageRandomWalk = new QWidget(this);
        methodStackedWidget->addWidget(pageRandomWalk);
        pageRandomWalkLayout = new QVBoxLayout(pageRandomWalk);

        pageRandomWalkLayout->addWidget(new QLabel("Stop Eps."));
        randomWalkStopEps = new QLineEdit(this);
        randomWalkStopEps->setText("0.00001");
        pageRandomWalkLayout->addWidget(randomWalkStopEps);

        pageRandomWalkLayout->addWidget(new QLabel("Max. Steps"));
        randomWalkSteps = new QLineEdit(this);
        randomWalkSteps->setText("1000");
        pageRandomWalkLayout->addWidget(randomWalkSteps);

        pageRandomWalkLayout->addWidget(new QLabel("Min. Steps"));
        randomWalkMinSteps = new QLineEdit(this);
        randomWalkMinSteps->setText("100");
        pageRandomWalkLayout->addWidget(randomWalkMinSteps);

        pageRandomWalkLayout->addWidget(new QLabel("P"));
        randomWalkP = new QLineEdit(this);
        randomWalkP->setText("0.2");
        pageRandomWalkLayout->addWidget(randomWalkP);

        pageRandomWalkLayout->addWidget(new QLabel("Delta"));
        randomWalkDelta = new QLineEdit(this);
        randomWalkDelta->setText("0.1s");
        pageRandomWalkLayout->addWidget(randomWalkDelta);


        connect(comboBox,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                methodStackedWidget,
                &QStackedWidget::setCurrentIndex
        );
        return groupBox;
    }

    QWidget* pageFunctionAckey;

    QVBoxLayout* pageFunctionAckeyLayout;

    QGroupBox* createGroupFunction() {
        const auto groupBox = new QGroupBox(tr("Test Function and Area"));
        const auto vboxLayout = new QVBoxLayout(this);
        groupBox->setLayout(vboxLayout);

        auto* comboBox = new QComboBox(this);
        vboxLayout->addWidget(comboBox);

        functionStackedWidget = new QStackedWidget(this);
        vboxLayout->addWidget(functionStackedWidget);


        // Page 1, Himmelblau
        comboBox->addItem(tr("Himmelblau function // R^2"));
        pageFunctionHimmelblau = new QWidget(this);
        functionStackedWidget->addWidget(pageFunctionHimmelblau);
        pageFunctionHimmelblauLayout = new QVBoxLayout(pageFunctionHimmelblau);
        pageFunctionHimmelblauLayout->addWidget(
            new QLabel("f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2")
        );
        CoordinateWidget::to("X", "-5", "5", pageFunctionHimmelblauLayout, functionHimmelblauArgArea, this);
        CoordinateWidget::to("Y", "-5", "5", pageFunctionHimmelblauLayout, functionHimmelblauArgArea, this);

        // Page 1, Himmelblau
        comboBox->addItem(tr("Styblinski–Tang function // R^2"));
        pageFunctionAckey = new QWidget(this);
        functionStackedWidget->addWidget(pageFunctionAckey);
        pageFunctionAckeyLayout = new QVBoxLayout(pageFunctionAckey);
        pageFunctionAckeyLayout->addWidget(
            new QLabel(
                R"(f(x, y) = \sum_{i=0}^n{x_i^4 - 16 x_i^2 + 5 x_i} / 2)")
        );
        CoordinateWidget::to("X", "-5", "5", pageFunctionAckeyLayout, functionAckeyArgArea, this);
        CoordinateWidget::to("Y", "-5", "5", pageFunctionAckeyLayout, functionAckeyArgArea, this);

        // Page 2, Rastrigin
        comboBox->addItem(tr("Rastrigin function  // R^2"));
        pageFunctionRastrigin2 = new QWidget(this);
        functionStackedWidget->addWidget(pageFunctionRastrigin2);
        pageFunctionRastriginLayout2 = new QVBoxLayout(pageFunctionRastrigin2);
        pageFunctionRastriginLayout2->addWidget(
            new QLabel("f(x) = 10n + \\sum_{i=1}^{2} (x_i^2 - 10 * cos(2 \\pi x_i))")
        );
        CoordinateWidget::to("X", "-5", "5", pageFunctionRastriginLayout2, functionRastrigin2ArgArea, this);
        CoordinateWidget::to("Y", "-5", "5", pageFunctionRastriginLayout2, functionRastrigin2ArgArea, this);

        // Page 3, Rastrigin
        comboBox->addItem(tr("Rastrigin function  // R^3"));
        pageFunctionRastrigin3 = new QWidget(this);
        functionStackedWidget->addWidget(pageFunctionRastrigin3);
        pageFunctionRastriginLayout3 = new QVBoxLayout(pageFunctionRastrigin3);
        pageFunctionRastriginLayout3->addWidget(
            new QLabel("f(x) = 10n + \\sum_{i=1}^{3} (x_i^2 - 10 * cos(2 \\pi x_i))")
        );
        CoordinateWidget::to("X", "-5", "5", pageFunctionRastriginLayout3, functionRastrigin3ArgArea, this);
        CoordinateWidget::to("Y", "-5", "5", pageFunctionRastriginLayout3, functionRastrigin3ArgArea, this);
        CoordinateWidget::to("Z", "-5", "5", pageFunctionRastriginLayout3, functionRastrigin3ArgArea, this);

        // Page 4, Rastrigin
        comboBox->addItem(tr("Rastrigin function  // R^4"));
        pageFunctionRastrigin4 = new QWidget(this);
        functionStackedWidget->addWidget(pageFunctionRastrigin4);
        pageFunctionRastriginLayout4 = new QVBoxLayout(pageFunctionRastrigin4);
        pageFunctionRastriginLayout4->addWidget(
            new QLabel("f(x) = 10n + \\sum_{i=1}^{4} (x_i^2 - 10 * cos(2 \\pi x_i))")
        );
        CoordinateWidget::to("X", "-5", "5", pageFunctionRastriginLayout4, functionRastrigin4ArgArea, this);
        CoordinateWidget::to("Y", "-5", "5", pageFunctionRastriginLayout4, functionRastrigin4ArgArea, this);
        CoordinateWidget::to("Z", "-5", "5", pageFunctionRastriginLayout4, functionRastrigin4ArgArea, this);
        CoordinateWidget::to("W", "-5", "5", pageFunctionRastriginLayout4, functionRastrigin4ArgArea, this);

        connect(comboBox,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                functionStackedWidget,
                &QStackedWidget::setCurrentIndex
        );
        return groupBox;
    }

    QGroupBox* createGraphConfiguration() {
        flagDrawGraph = new QGroupBox(tr("Draw Graph (only for 2d)"));
        flagDrawGraph->setCheckable(true);
        flagDrawGraph->setChecked(true);

        auto* vbox = new QVBoxLayout;
        drawPixelSize = new QLineEditWithLabel("Pixel Size", "8", this);
        vbox->addWidget(drawPixelSize);
        drawGraphDensity = new QLineEditWithLabel("Sample Density", "3", this);
        vbox->addWidget(drawGraphDensity);
        vbox->addStretch(1);
        flagDrawGraph->setLayout(vbox);

        return flagDrawGraph;
    }

    QGroupBox* other;
    QLineEditWithLabel* otherSeed;

    QGroupBox* createOther() {
        other = new QGroupBox(tr("Other Settings"));

        auto* vbox = new QVBoxLayout;
        otherSeed = new QLineEditWithLabel("Random Seed", "0", this);
        vbox->addWidget(otherSeed);

        other->setLayout(vbox);
        return other;
    }
};


#endif //GUI_SETTINGS_H
