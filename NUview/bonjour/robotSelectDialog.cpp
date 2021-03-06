#include <QtGui>
#include "robotSelectDialog.h"
#include "bonjourservicebrowser.h"
#include "bonjourserviceresolver.h"
#include <QDebug>

robotSelectDialog::robotSelectDialog(QWidget * parent, const QString& service): QDialog(parent)
{
    bonjourBrowser = new BonjourServiceBrowser(this);
    treeWidget = new QTreeWidget(this);
    QString serviceDisplay(service);
    QStringList tempList = serviceDisplay.split('.');
    if(!tempList.empty())
    {
        serviceDisplay = tempList.first();
        serviceDisplay.remove('_');
    }
    treeWidget->setHeaderLabels(QStringList() << tr("Available %1 services").arg(serviceDisplay));

    connect(bonjourBrowser, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
                this, SLOT(updateRecords(const QList<BonjourRecord> &)));

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    connectButton->setEnabled(false);

    cancelButton = new QPushButton(tr("Cancel"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveSelected()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(treeWidget, 0, 0, 2, 2);
//    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Robot Selection"));
    treeWidget->setFocus();
    m_service = service;
    refresh();
}

void robotSelectDialog::saveSelected()
{
    QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
    if(selectedItems.length() > 0)
    {
        QTreeWidgetItem *item = selectedItems.at(0);
        QVariant variant = item->data(1, Qt::UserRole);
        m_selectedHost = variant.value<BonjourRecord>();
    }
    else
    {
        m_selectedHost = BonjourRecord();
    }
}


void robotSelectDialog::enableConnectButton()
{
    connectButton->setEnabled(treeWidget->invisibleRootItem()->childCount() != 0);
}

void robotSelectDialog::updateRecords(const QList<BonjourRecord> &list)
{
    treeWidget->clear();
    treeWidget->setIconSize(QSize(36,36));
    QIcon robotIcon(QString(":/icons/Robot-icon.png"));
    foreach (BonjourRecord record, list) {
        QVariant variant;
        variant.setValue(record);
        QTreeWidgetItem *processItem = new QTreeWidgetItem(treeWidget,
                                                           QStringList() << record.serviceName);
        processItem->setIcon(0,robotIcon);
        processItem->setData(1, Qt::UserRole, variant);
    }

    if (treeWidget->invisibleRootItem()->childCount() > 0) {
        treeWidget->invisibleRootItem()->child(0)->setSelected(true);
    }
    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(0,Qt::AscendingOrder);
    treeWidget->setSortingEnabled(false);
    enableConnectButton();
}

void robotSelectDialog::refresh()
{
    bonjourBrowser->browseForServiceType(m_service);
}
