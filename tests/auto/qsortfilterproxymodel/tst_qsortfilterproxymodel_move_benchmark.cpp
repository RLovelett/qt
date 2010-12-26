
#include <QtTest/QtTest>

#include <QtCore>
#include <QtGui>

#include <qdebug.h>

#include "dynamictreemodel.h"
#include "modeldumper.h"
#include "modeltest.h"

class tst_QSortFilterProxyModelMoveBenchmark : public QObject
{
  Q_OBJECT
public:
  tst_QSortFilterProxyModelMoveBenchmark(QObject *parent = 0);
  virtual ~tst_QSortFilterProxyModelMoveBenchmark();

private slots:
  void testDataChange_data();
  void testDataChange();

  void testRowMove_data();
  void testRowMove();

  void testCustomRoleDataChange_data();
  void testCustomRoleDataChange();

private:
  DynamicTreeModel *createModel();
};

tst_QSortFilterProxyModelMoveBenchmark::tst_QSortFilterProxyModelMoveBenchmark(QObject *parent)
  : QObject(parent)
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
}

tst_QSortFilterProxyModelMoveBenchmark::~tst_QSortFilterProxyModelMoveBenchmark()
{

}

static QList<int> getRows(const QModelIndex &_index)
{
    QList<int> rows;

    QModelIndex index = _index;
    while (index.isValid())
    {
        rows.prepend(index.row());
        index = index.parent();
    }
    return rows;
}

static void addLevel(DynamicTreeModel *model, const QModelIndex &parent, int length )
{
    enum { width = 4 };
    ModelInsertCommand ins(model);
    ins.setAncestorRowNumbers(getRows(parent));
    ins.setNumCols(width);
    ins.setStartRow(0);
    ins.setEndRow(length - 1);
    ins.doCommand();
}

DynamicTreeModel* tst_QSortFilterProxyModelMoveBenchmark::createModel()
{
  DynamicTreeModel *model = new DynamicTreeModel(this);

  addLevel(model, QModelIndex(), 10);

  {
    QModelIndex parent = model->index(3, 0);
    addLevel(model, parent, 100);
    for (int row = 0; row < 100; row += 4) {
      QModelIndex index = model->index(row, 0, parent);
      addLevel(model, index, 100);

      for (int row2 = 0; row2 < 100; row2 += 33) {
        QModelIndex child = model->index(row2, 0, index);
        Q_ASSERT(child.isValid());
        addLevel(model, child, 10);
      }
    }
  }
  {
    QModelIndex parent = model->index(7, 0);
    addLevel(model, parent, 100);
    for (int row = 0; row < 100; row += 20) {
      QModelIndex index = model->index(row, 0, parent);
      addLevel(model, index, 50);
      for (int row2 = 0; row2 < 50; row2 += 10) {
        QModelIndex child = model->index(row2, 0, index);
        Q_ASSERT(child.isValid());
        addLevel(model, child, 10);
      }
    }
  }
  return model;
}

static QList<QPersistentModelIndex> create_mappings(QAbstractItemModel *model, const QModelIndex &parent = QModelIndex(), bool per = true)
{
    const int rowCount = model->rowCount(parent);
    QList<QPersistentModelIndex> list;

    for(int row = 0; row < rowCount; ++row ) {
        static const int column = 0;
        const QModelIndex idx = model->index(row, column, parent);
        if (model->rowCount(idx) != 0) {
            if (per)
              list << QPersistentModelIndex(idx);
        }
        list << create_mappings(model, idx);

    }
    return list;
}

#define SETUP(object)                   \
    object->setObjectName(#object);     \
    object->setDynamicSortFilter(true); \
    object->sort(0);                    \

void tst_QSortFilterProxyModelMoveBenchmark::testDataChange_data()
{
  QTest::addColumn<bool>("manySiblings");

  QTest::newRow("manySiblings") << true;
  QTest::newRow("fewSiblings") << false;
}

void tst_QSortFilterProxyModelMoveBenchmark::testDataChange()
{
    QFETCH(bool, manySiblings);

    DynamicTreeModel *model = createModel();

    QSortFilterProxyModel *p1_1 = new QSortFilterProxyModel(this);
    p1_1->setSourceModel(model);
    QSortFilterProxyModel *p2_1 = new QSortFilterProxyModel(this);
    p2_1->setSourceModel(p1_1);

    SETUP(p1_1)
    SETUP(p2_1)

    QList<QPersistentModelIndex> list;
    list << create_mappings(p1_1);
    list << create_mappings(p2_1);

    ModelDataChangeCommand changeCommand(model);
    changeCommand.setAncestorRowNumbers(QList<int>() << (manySiblings ? 3 : 7));
    changeCommand.setStartRow(0);
    changeCommand.setEndRow(0);

    QBENCHMARK {
      changeCommand.doCommand();
    }
}

void tst_QSortFilterProxyModelMoveBenchmark::testRowMove_data()
{
  QTest::addColumn<bool>("manySiblings");

  QTest::newRow("manySiblings") << true;
  QTest::newRow("fewSiblings") << false;
}

void tst_QSortFilterProxyModelMoveBenchmark::testRowMove()
{
    QFETCH(bool, manySiblings);

    DynamicTreeModel *model = createModel();

    QSortFilterProxyModel *p1_1 = new QSortFilterProxyModel(this);
    p1_1->setSourceModel(model);
    QSortFilterProxyModel *p2_1 = new QSortFilterProxyModel(this);
    p2_1->setSourceModel(p1_1);

    SETUP(p1_1)
    SETUP(p2_1)

    QList<QPersistentModelIndex> list;
    list << create_mappings(p1_1);
    list << create_mappings(p2_1);

    ModelMoveCommand moveCommand(model, 0);
    moveCommand.setAncestorRowNumbers(QList<int>() << (manySiblings ? 3 : 7));
    moveCommand.setStartRow(0);
    moveCommand.setEndRow(0);
    moveCommand.setDestRow(0);

    QBENCHMARK {
      moveCommand.doCommand();
    }
}

void tst_QSortFilterProxyModelMoveBenchmark::testCustomRoleDataChange_data()
{
  QTest::addColumn<bool>("manySiblings");

  QTest::newRow("manySiblings") << true;
  QTest::newRow("fewSiblings") << false;
}

void tst_QSortFilterProxyModelMoveBenchmark::testCustomRoleDataChange()
{
    QFETCH(bool, manySiblings);

    DynamicTreeModel *model = createModel();

    QSortFilterProxyModel *p1_1 = new QSortFilterProxyModel(this);
    p1_1->setSourceModel(model);
    QSortFilterProxyModel *p2_1 = new QSortFilterProxyModel(this);
    p2_1->setSourceModel(p1_1);

    SETUP(p1_1)
    SETUP(p2_1)

    QList<QPersistentModelIndex> list;
    list << create_mappings(p1_1);
    list << create_mappings(p2_1);

    ModelDataChangeCommand dataChangeCommand(model, 0);
    dataChangeCommand.setAncestorRowNumbers(QList<int>() << (manySiblings ? 3 : 7));
    dataChangeCommand.setStartRow(0);
    dataChangeCommand.setEndRow(0);
    dataChangeCommand.setRole(DynamicTreeModel::CustomDataRole);

    QBENCHMARK {
      dataChangeCommand.doCommand();
    }
}

QTEST_MAIN(tst_QSortFilterProxyModelMoveBenchmark)
#include "tst_qsortfilterproxymodel_move_benchmark.moc"
