#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QtXml/QDomDocument>

#include "Shared/Logger.h"
#include "Common/Constants.h"

#include "Dataset.h"
#include "DatasetDefinition.h"

const int DatasetDefinition::sampleSize_ = 10;

DatasetDefinition::DatasetDefinition(QString name):
    valid_(false),
    rowsCount_(0),
    columnsCount_(0),
    name_(std::move(name)),
    emptyColName_(QObject::tr("no name"))
{

}

int DatasetDefinition::rowCount() const
{
    return rowsCount_;
}

int DatasetDefinition::columnCount() const
{
    return columnsCount_;
}

bool DatasetDefinition::isSpecialColumnTagged(SpecialColumn column) const
{
    return specialColumns_.contains(column);
}

bool DatasetDefinition::getSpecialColumnIfExists(SpecialColumn columnTag,
                                                 int& column) const
{
    if (isSpecialColumnTagged(columnTag))
    {
        column = specialColumns_[columnTag];
        return true;
    }
    return false;
}

QString DatasetDefinition::getColumnName(int column) const
{
    if (columnsCount_ - 1 >= column)
    {
        return headerColumnNames_[column];
    }

    Q_ASSERT(false);
    return QLatin1String("");
}

QString DatasetDefinition::dumpDatasetDefinition() const
{
    QString dump;
    for (int i = 0; i < columnsCount_; ++i)
    {
        dump += "Column " + QString::number(i) + " name=" + headerColumnNames_.at(i);
        dump += " format=" + QString::number(static_cast<int>(columnsFormat_.at(i)));
        if (!activeColumns_.isEmpty())
        {
            static const QString notActive(QStringLiteral(" not active"));
            static const QString active(QStringLiteral(" active"));
            dump += " active=" + (activeColumns_[i] ? active : notActive);
        }

        QMapIterator<SpecialColumn, int> it(specialColumns_);
        if (it.findNext(i))
        {
            dump += " special=" + QString::number(static_cast<int>(it.key()));
        }
        dump.append(QString::fromLatin1("\n"));
    }
    return dump;
}

QString DatasetDefinition::getName() const
{
    return name_;
}

const QString DatasetDefinition::getError() const
{
    return error_;
}

void DatasetDefinition::clearSampleData()
{
    sampleData_.clear();
    sampleData_.resize(0);
}

const QVector<QVector<QVariant> >* DatasetDefinition::getSampleData() const
{
    return &sampleData_;
}

void DatasetDefinition::setActiveColumns(const QVector<bool>& activeColumns)
{
    activeColumns_ = activeColumns;
}

const QVector<bool>* DatasetDefinition::getActiveColumns() const
{
    return &activeColumns_;
}

int DatasetDefinition::getActiveColumnCount() const
{
    int activeColumnCount = 0;

    for (bool activeColumn : activeColumns_)
    {
        if (activeColumn)
        {
            activeColumnCount++;
        }
    }

    return activeColumnCount;
}

void DatasetDefinition::rebuildDefinitonUsingActiveColumnsOnly()
{
    QVector<DataFormat> tempColumnsFormat;

    QStringList tempHeaderColumnNames = QStringList();

    QMap<SpecialColumn, int> specialColumnsTemp = QMap<SpecialColumn, int>();

    int activeColumnNumber = 0;

    bool specialColumnDateMarked =
        isSpecialColumnTagged(SPECIAL_COLUMN_TRANSACTION_DATE);
    bool specialColumnPriceMarked =
        isSpecialColumnTagged(SPECIAL_COLUMN_PRICE_PER_UNIT);

    for (int i = 0; i < activeColumns_.count(); ++i)
    {
        if (activeColumns_[i])
        {
            tempColumnsFormat.push_back(columnsFormat_[i]);
            tempHeaderColumnNames << headerColumnNames_[i];

            if (specialColumnDateMarked &&
                specialColumns_[SPECIAL_COLUMN_TRANSACTION_DATE] == i)
            {
                specialColumnsTemp[SPECIAL_COLUMN_TRANSACTION_DATE] =
                    activeColumnNumber;
            }

            if (specialColumnPriceMarked &&
                specialColumns_[SPECIAL_COLUMN_PRICE_PER_UNIT] == i)
            {
                specialColumnsTemp[SPECIAL_COLUMN_PRICE_PER_UNIT] =
                    activeColumnNumber;
            }

            activeColumnNumber++;
        }
    }

    columnsFormat_ = tempColumnsFormat;
    headerColumnNames_ = tempHeaderColumnNames;
    specialColumns_ = specialColumnsTemp;

    //Set new column count.
    columnsCount_ = getActiveColumnCount();

    //Those info is not needed anymore.
    activeColumns_.clear();
}

void DatasetDefinition::setSpecialColumn(SpecialColumn columnTag, int column)
{
    specialColumns_[columnTag] = column;
}

QVariant DatasetDefinition::getDefaultVariantForFormat(const DataFormat format) const
{
    switch (format)
    {
        case DATA_FORMAT_STRING:
        {
            return QVariant(QVariant::Int);
        }

        case DATA_FORMAT_FLOAT:
        {
            return QVariant(QVariant::Double);
        }
        case DATA_FORMAT_DATE:
        {
            return QVariant(QVariant::Date);
        }

        case DATA_FORMAT_UNKNOWN:
        {
            Q_ASSERT(false);
            return QVariant(QVariant::String);
        }
    }
}
