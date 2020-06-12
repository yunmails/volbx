#include <QtTest/QtTest>

#include "ConfigurationTest.h"
#include "InnerTests.h"
#include "PlotDataProviderTest.h"
#include "SpreadsheetsTest.h"

int main(int argc, char* argv[])
{
    QLocale::setDefault(QLocale::c());

    QApplication a(argc, argv);

    ConfigurationTest configurationTest;
    QTest::qExec(&configurationTest);

    SpreadsheetsTest spreadsheetsTest;
    QTest::qExec(&spreadsheetsTest);

    PlotDataProviderTest plotDataProviderTest;
    QTest::qExec(&plotDataProviderTest);

    InnerTests ineerTests;
    QTest::qExec(&ineerTests);

    return 0;
}
