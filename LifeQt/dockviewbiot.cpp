#include "dockviewbiot.h"
#include "core/Biots.h"
#include "ui_dockviewbiot.h"
#include <QHeaderView>
#include <QSet>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <rapidjson/document.h>

using namespace rapidjson;

static QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parent, const QString &name, const QString &value = QString())
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setText(1, value);
    parent->addChild(item);
    return item;
}

static QTreeWidgetItem *AddTreeItem(QTreeWidget *tree, const QString &name, const QString &value = QString())
{
    QTreeWidgetItem *item = new QTreeWidgetItem(tree);
    item->setText(0, name);
    item->setText(1, value);
    return item;
}

static QString JsonScalarToString(const Value &value)
{
    if(value.IsString())
        return QString::fromUtf8(value.GetString(), value.GetStringLength());
    if(value.IsBool())
        return value.GetBool() ? "true" : "false";
    if(value.IsInt())
        return QString::number(value.GetInt());
    if(value.IsUint())
        return QString::number(value.GetUint());
    if(value.IsInt64())
        return QString::number(value.GetInt64());
    if(value.IsUint64())
        return QString::number(value.GetUint64());
    if(value.IsDouble())
        return QString::number(value.GetDouble(), 'g', 12);
    if(value.IsNull())
        return "null";
    return QString();
}

static void AddJsonValue(QTreeWidgetItem *parent, const QString &name, const Value &value)
{
    if(value.IsObject())
    {
        QTreeWidgetItem *item = AddTreeItem(parent, name, QString("{%1}").arg(value.MemberCount()));
        for(Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it)
            AddJsonValue(item, QString::fromUtf8(it->name.GetString(), it->name.GetStringLength()), it->value);
    }
    else if(value.IsArray())
    {
        QTreeWidgetItem *item = AddTreeItem(parent, name, QString("[%1]").arg(value.Size()));
        for(SizeType i = 0; i < value.Size(); i++)
            AddJsonValue(item, QString("[%1]").arg(i), value[i]);
    }
    else
    {
        AddTreeItem(parent, name, JsonScalarToString(value));
    }
}

static void CollectExpandedItems(QTreeWidgetItem *item, const QString &path, QSet<QString> &expanded)
{
    if(item->isExpanded())
        expanded.insert(path);

    for(int i = 0; i < item->childCount(); i++)
        CollectExpandedItems(item->child(i), path + "/" + item->child(i)->text(0), expanded);
}

static void RestoreExpandedItems(QTreeWidgetItem *item, const QString &path, const QSet<QString> &expanded)
{
    item->setExpanded(expanded.contains(path));

    for(int i = 0; i < item->childCount(); i++)
        RestoreExpandedItems(item->child(i), path + "/" + item->child(i)->text(0), expanded);
}

static void AddRuntimeGeometry(QTreeWidgetItem *root, Biot *pBiot)
{
    AddTreeItem(root, "origin", QString("%1, %2").arg(pBiot->origin.x()).arg(pBiot->origin.y()));
    AddTreeItem(root, "bounds", QString("left=%1 top=%2 right=%3 bottom=%4")
                    .arg(pBiot->leftX).arg(pBiot->topY).arg(pBiot->rightX).arg(pBiot->bottomY));
    AddTreeItem(root, "totalDistance", QString::number(pBiot->totalDistance));
    AddTreeItem(root, "turnBenefit", QString::number(pBiot->turnBenefit));
    AddTreeItem(root, "internalState", QString::number(pBiot->m_internalState));

    QTreeWidgetItem *colors = AddTreeItem(root, "colorDistance");
    AddTreeItem(colors, "green", QString::number(pBiot->colorDistance[GREEN_LEAF]));
    AddTreeItem(colors, "blue", QString::number(pBiot->colorDistance[BLUE_LEAF]));
    AddTreeItem(colors, "red", QString::number(pBiot->colorDistance[RED_LEAF]));
    AddTreeItem(colors, "lightBlue", QString::number(pBiot->colorDistance[LBLUE_LEAF]));
    AddTreeItem(colors, "white", QString::number(pBiot->colorDistance[WHITE_LEAF]));

    QTreeWidgetItem *segments = AddTreeItem(root, "segments", QString("[%1]").arg(pBiot->genes));
    for(int i = 0; i < pBiot->genes && i < MAX_GENES; i++)
    {
        QTreeWidgetItem *segment = AddTreeItem(segments, QString("[%1]").arg(i));
        AddTreeItem(segment, "line", QString::number(pBiot->lineNo[i]));
        AddTreeItem(segment, "gene", QString::number(pBiot->geneNo[i]));
        AddTreeItem(segment, "type", QString::number(pBiot->nType[i]));
        AddTreeItem(segment, "state", QString::number(pBiot->state[i]));
        AddTreeItem(segment, "distance", QString::number(pBiot->distance[i]));
        AddTreeItem(segment, "start", QString("%1, %2").arg(pBiot->startPt[i].x()).arg(pBiot->startPt[i].y()));
        AddTreeItem(segment, "stop", QString("%1, %2").arg(pBiot->stopPt[i].x()).arg(pBiot->stopPt[i].y()));
    }
}

EnvironmentListenerToDockViewBiot::EnvironmentListenerToDockViewBiot()
{
    dockViewBiot = nullptr;

}

EnvironmentListenerToDockViewBiot::~EnvironmentListenerToDockViewBiot()
{

}

void EnvironmentListenerToDockViewBiot::BiotUpdated(Biot* pBiot)
{
    if(dockViewBiot)
        dockViewBiot->BiotUpdated(pBiot);
}

// ***********************************************

DockViewBiot::DockViewBiot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DockViewBiot),
    detailsTree(nullptr)
{
    ui->setupUi(this);
    ui->gridLayout->removeItem(ui->verticalSpacer);
    delete ui->verticalSpacer;
    ui->verticalSpacer = nullptr;

    detailsTree = new QTreeWidget(this);
    detailsTree->setColumnCount(2);
    detailsTree->setHeaderLabels(QStringList() << "Field" << "Value");
    detailsTree->setAlternatingRowColors(true);
    detailsTree->setUniformRowHeights(true);
    detailsTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    detailsTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->verticalLayout->addWidget(detailsTree, 1);

    currentBiotId = 0;
    env = nullptr;
    listenAdapter.dockViewBiot = this;
}

DockViewBiot::~DockViewBiot()
{
    delete ui;
}

void DockViewBiot::SelectedBiot(uint32_t biotId)
{

    currentBiotId = biotId;
}

void DockViewBiot::BiotUpdated(Biot* pBiot)
{
    if(pBiot->m_Id != currentBiotId) return;

    this->ui->nameEdit->setText(pBiot->GetName().c_str());
    this->ui->generationEdit->setText(QString::asprintf("%d", pBiot->m_generation));

    //this->ui->speciesComboBox->setEditText(pBiot->);
    if(pBiot->trait.IsAsexual())
    {
        this->ui->orientationComboBox->setEditText("Asexual");
    }
    else
    {
        if(pBiot->trait.IsMale())
            this->ui->orientationComboBox->setEditText("Male");
        else
            this->ui->orientationComboBox->setEditText("Female");
    }
    this->ui->daysOldEdit->setText(QString::asprintf("%f", pBiot->m_age * 0.05 / CEnvStats::SAMPLE_TIME));
    this->ui->lifespanEdit->setText(QString::asprintf("%f", pBiot->m_maxAge * 0.05 / CEnvStats::SAMPLE_TIME));
    this->ui->energyEdit->setText(QString::asprintf("%ld", pBiot->energy));
    this->ui->sickTimeEdit->setText(QString::asprintf("%d", pBiot->m_nSick));

    this->ui->childredEdit->setText(pBiot->m_sWorldName.c_str());
    //this->ui->fertileEdit->setText(pBiot->fert);
    this->ui->childredEdit->setText(QString::asprintf("%d", pBiot->m_totalChildren));

    UpdateDetailsTree(pBiot);
}

void DockViewBiot::UpdateDetailsTree(Biot* pBiot)
{
    QSet<QString> expanded;
    for(int i = 0; i < detailsTree->topLevelItemCount(); i++)
        CollectExpandedItems(detailsTree->topLevelItem(i), detailsTree->topLevelItem(i)->text(0), expanded);

    detailsTree->setUpdatesEnabled(false);
    detailsTree->clear();

    QTreeWidgetItem *runtimeRoot = AddTreeItem(detailsTree, "Runtime");
    AddRuntimeGeometry(runtimeRoot, pBiot);

    QTreeWidgetItem *jsonRoot = AddTreeItem(detailsTree, "Saved JSON");
    try
    {
        Document d;
        d.SetObject();
        Value biotJson(kObjectType);
        pBiot->SerializeJson(d, biotJson);
        AddJsonValue(jsonRoot, "biot", biotJson);
    }
    catch (const std::exception &err)
    {
        AddTreeItem(jsonRoot, "error", err.what());
    }

    if(expanded.isEmpty())
    {
        runtimeRoot->setExpanded(true);
        jsonRoot->setExpanded(true);
    }
    else
    {
        for(int i = 0; i < detailsTree->topLevelItemCount(); i++)
            RestoreExpandedItems(detailsTree->topLevelItem(i), detailsTree->topLevelItem(i)->text(0), expanded);
    }

    detailsTree->setUpdatesEnabled(true);
}

void DockViewBiot::on_applyButton_clicked()
{

}
