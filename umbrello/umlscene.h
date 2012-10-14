/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2012                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

#ifndef UMLSCENE_H
#define UMLSCENE_H

// local includes
#include "associationwidgetlist.h"
#include "basictypes.h"
#include "messagewidgetlist.h"
#include "optionstate.h"
#include "umlobject.h"
#include "umlobjectlist.h"
#include "umlwidgetlist.h"
#include "worktoolbar.h"

// Qt includes
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDomDocument>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPolygonItem>

// forward declarations
class ClassOptionsPage;
class IDChangeLog;
class LayoutGrid;
class ListPopupMenu;
class FloatingTextWidget;
class ObjectWidget;
class ToolBarState;
class ToolBarStateFactory;
class UMLFolder;
class UMLApp;
class UMLDoc;
class UMLAttribute;
class UMLCanvasObject;
class UMLClassifier;
class UMLViewImageExporter;
class UMLForeignKeyConstraint;
class UMLEntity;
class UMLView;

class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QHideEvent;
class QMouseEvent;
class QPrinter;
class QShowEvent;
class UMLScene;

/// uml related types - makes it easier to switch to QGraphicsScene types
// base types
typedef QPointF UMLScenePoint;
typedef QRectF UMLSceneRect;
typedef QSizeF UMLSceneSize;
typedef QLineF UMLSceneLine;
typedef qreal UMLSceneValue;

class UMLScenePolygon : public QPolygonF
{
public:
    UMLScenePolygon() {}
    void setPoint(int index, const UMLScenePoint &p)
    {
        replace(index, p);
    }
};

// event types
typedef QKeyEvent UMLSceneKeyEvent;
typedef QGraphicsSceneHoverEvent UMLSceneHoverEvent;
typedef QGraphicsSceneContextMenuEvent UMLSceneContextMenuEvent;
typedef QGraphicsSceneDragDropEvent UMLSceneDragDropEvent;
typedef QGraphicsSceneDragDropEvent UMLSceneDragEnterEvent;
typedef QGraphicsSceneDragDropEvent UMLSceneDragMoveEvent;

#define UMLSceneMouseEvent QGraphicsSceneMouseEvent

class UMLSceneMoveEvent : public QGraphicsSceneMoveEvent
{
public:
    UMLSceneMoveEvent() {}

    UMLScenePoint pos()
    {
        return newPos();
    }
};

// Qt3 migration wrapper for QGraphicsScene items 
typedef QGraphicsItem UMLSceneItem;
typedef QList<QGraphicsItem*> UMLSceneItemList;

class UMLSceneLineItem : public QGraphicsLineItem
{
public:
    UMLSceneLineItem()
    : QGraphicsLineItem(0)
    {
    }

    void setPoints(UMLSceneValue x1, UMLSceneValue y1, UMLSceneValue x2, UMLSceneValue y2)
    {
        setLine(x1, y1, x2, y2);
    }

    UMLScenePoint startPoint()
    {
        return line().p1();
    }
    
    UMLScenePoint endPoint()
    {
        return line().p2();
    }

    void setZ(UMLSceneValue z)
    {
        setZValue(z);
    }

    void setCanvas(QGraphicsScene *scene)
    {
        scene->addItem(this);
    }
};

class UMLSceneRectItem : public QGraphicsRectItem
{
public:
    UMLSceneRectItem()
    : QGraphicsRectItem(0)
    {
    }
    
    UMLSceneRectItem(int x, int y, int w, int h)
    : QGraphicsRectItem(x, y, w, h, 0)
    {
    }

    UMLSceneValue z() const
    {
        return zValue();
    }

    void setZ(UMLSceneValue z)
    {
        setZValue(z);
    }

    UMLSceneSize size() const
    {
        return rect().size();
    }

    void setSize(UMLSceneValue w, UMLSceneValue h)
    {
        setRect(rect().x(), rect().y(), w, h);
    }

    void setCanvas(QGraphicsScene *scene)
    {
        scene->addItem(this);
    }
};

class UMLScenePolygonItem : public QGraphicsPolygonItem
{
public:
    UMLScenePolygonItem()
    : QGraphicsPolygonItem(0)
    {
    }

    void setZ(UMLSceneValue z)
    {
        setZValue(z);
    }

    void setPoints(const UMLScenePolygon &p)
    {
        setPolygon(p);
    }

    void setCanvas(QGraphicsScene *scene)
    {
        scene->addItem(this);
    }

};

class UMLSceneEllipseItem : public QGraphicsEllipseItem
{
public:
    UMLSceneEllipseItem()
    : QGraphicsEllipseItem(0)
    {
    }

    UMLSceneEllipseItem(int width, int height)
    : QGraphicsEllipseItem(0, 0, width, height, 0)
    {
    }

    void setCanvas(QGraphicsScene *scene)
    {
        scene->addItem(this);
    }

};

/**
 * UMLScene instances represent diagrams.
 * The UMLScene class inherits from QGraphicsScene and it owns the
 * objects displayed (see m_WidgetList.)
 */
class UMLScene : public QGraphicsScene
{
    Q_OBJECT
public:
    friend class UMLViewImageExporterModel;

    UMLScene(UMLFolder *parentFolder, UMLView *view=0);
    virtual ~UMLScene();

    UMLScene *canvas()
    {
        return this;
    }

    UMLView* activeView() const;

    // Accessors and other methods dealing with loaded/saved data

    UMLFolder* folder() const;
    void setFolder(UMLFolder *folder);

    QString documentation() const;
    void setDocumentation(const QString &doc);

    QString name() const;
    void setName(const QString &name);

    Uml::DiagramType type() const;
    void setType(Uml::DiagramType type);

    Uml::IDType ID() const;
    void setID(Uml::IDType id);

    UMLScenePoint pos() const;
    void setPos(const UMLScenePoint &pos);

    const QColor& fillColor() const;
    void setFillColor(const QColor &color);

    const QColor& lineColor() const;
    void setLineColor(const QColor &color);

    uint lineWidth() const;
    void setLineWidth(uint width);

    const QColor& textColor() const;
    void setTextColor(const QColor& color);

    const QColor& gridDotColor() const;
    void setGridDotColor(const QColor& color);

    void setSize(UMLSceneValue width, UMLSceneValue height);

    bool snapToGrid() const;
    void setSnapToGrid(bool bSnap);

    bool snapComponentSizeToGrid() const;
    void setSnapComponentSizeToGrid(bool bSnap);

    int snapX() const;
    int snapY() const;
    void setSnapSpacing(int x, int y);

    UMLSceneValue snappedX(UMLSceneValue x);
    UMLSceneValue snappedY(UMLSceneValue y);

    bool isSnapGridVisible() const;
    void setSnapGridVisible(bool bShow);

    bool useFillColor() const;
    void setUseFillColor(bool ufc);

    QFont font() const;
    void setFont(QFont font, bool changeAllWidgets = false);

    bool showOpSig() const;
    void setShowOpSig(bool bShowOpSig);

    const Settings::OptionState& optionState() const;
    void setOptionState(const Settings::OptionState& options);

    AssociationWidgetList& associationList();
    UMLWidgetList& widgetList();
    MessageWidgetList& messageList();
    UMLObjectList umlObjects();

    bool isOpen() const;
    void setIsOpen(bool isOpen);

    // End of accessors and methods that only deal with loaded/saved data
    ////////////////////////////////////////////////////////////////////////

    void print(QPrinter *pPrinter, QPainter & pPainter);

    void hideEvent(QHideEvent *he);
    void showEvent(QShowEvent *se);

    void checkMessages(ObjectWidget * w);

    UMLWidget* findWidget(Uml::IDType id);

    AssociationWidget* findAssocWidget(Uml::IDType id);
    AssociationWidget* findAssocWidget(Uml::AssociationType at,
                                       UMLWidget *pWidgetA, UMLWidget *pWidgetB);
    AssociationWidget* findAssocWidget(UMLWidget *pWidgetA,
                                       UMLWidget *pWidgetB, const QString& roleNameB);

    void removeWidget(UMLWidget *o);

    void setSelected(UMLWidget *w, UMLSceneMouseEvent *me);
    UMLWidgetList selectedWidgets() const;
    void clearSelected();

    void moveSelectedBy(UMLSceneValue dX, UMLSceneValue dY);

    int selectedCount(bool filterText = false) const;

    void selectionUseFillColor(bool useFC);
    void selectionSetFont(const QFont &font);
    void selectionSetLineColor(const QColor &color);
    void selectionSetLineWidth(uint width);
    void selectionSetFillColor(const QColor &color);
    void selectionToggleShow(int sel);

    void deleteSelection();

    void selectAll();

    Uml::IDType localID();

    bool widgetOnDiagram(Uml::IDType id);

    bool isSavedInSeparateFile();

    void setMenu(const QPoint& pos);

    void resetToolbar();

    bool getPaste() const;
    void setPaste(bool paste);

    void activate();

    AssociationWidgetList selectedAssocs();
    UMLWidgetList selectedWidgetsExt(bool filterText = true);

    void activateAfterLoad(bool bUseLog = false);

    void endPartialWidgetPaste();
    void beginPartialWidgetPaste();

    void removeAssoc(AssociationWidget* pAssoc);
    void removeAssociations(UMLWidget* pWidget);
    void selectAssociations(bool bSelect);

    void getWidgetAssocs(UMLObject* Obj, AssociationWidgetList & Associations);

    void removeAllAssociations();

    void removeAllWidgets();

    void showDocumentation(bool overwrite = false);
    void showDocumentation(UMLObject* object, bool overwrite = false);
    void showDocumentation(UMLWidget* widget, bool overwrite = false);
    void showDocumentation(AssociationWidget* widget, bool overwrite = false);

    void updateDocumentation(bool clear);

    void getDiagram(const UMLSceneRect &rect, QPixmap & diagram);
    void getDiagram(const UMLSceneRect &area, QPainter & painter);

    void copyAsImage(QPixmap*& pix);

    UMLViewImageExporter* getImageExporter();

    bool addAssociation(AssociationWidget* pAssoc, bool isPasteOperation = false);

    void removeAssocInViewAndDoc(AssociationWidget* assoc);

    bool addWidget(UMLWidget * pWidget, bool isPasteOperation = false);

    UMLScenePoint getPastePoint();
    void resetPastePoint();

    void setStartedCut();

    void createAutoAssociations(UMLWidget * widget);
    void createAutoAttributeAssociations(UMLWidget *widget);
    void createAutoConstraintAssociations(UMLWidget* widget);

    void updateContainment(UMLCanvasObject *self);

    bool showPropDialog();

    void setClassWidgetOptions(ClassOptionsPage * page);

    void checkSelections();

    bool checkUniqueSelection();

    void clearDiagram();

    void applyLayout(const QString &actionText);

    void toggleSnapToGrid();
    void toggleSnapComponentSizeToGrid();
    void toggleShowGrid();

    void fileLoaded();

    void resizeCanvasToItems();

    // Load/Save interface:

    virtual void saveToXMI(QDomDocument & qDoc, QDomElement & qElement);
    virtual bool loadFromXMI(QDomElement & qElement);

    bool loadUISDiagram(QDomElement & qElement);
    UMLWidget* loadWidgetFromXMI(QDomElement& widgetElement);

    void addObject(UMLObject *object);

    void selectWidgets(UMLSceneValue px, UMLSceneValue py, UMLSceneValue qx, UMLSceneValue qy);
    void selectWidgets(UMLWidgetList &widgets);
    void selectWidgetsOfAssoc(AssociationWidget *a);

    ObjectWidget * onWidgetLine(const UMLScenePoint &point) const;
    ObjectWidget * onWidgetDestructionBox(const UMLScenePoint &point) const;

    UMLWidget* getFirstMultiSelectedWidget() const;

    UMLWidget* widgetAt(const UMLScenePoint& p);
    AssociationWidget* associationAt(const UMLScenePoint& p);

    void setupNewWidget(UMLWidget *w);

    bool getCreateObject() const;
    void setCreateObject(bool bCreate);

    /**
     * Emit the sigRemovePopupMenu Qt signal.
     */
    void emitRemovePopupMenu() {
        emit sigRemovePopupMenu();
    }

    int generateCollaborationId();

    UMLSceneItemList collisions(const UMLScenePoint &p);

protected:
    // Methods and members related to loading/saving

    bool loadWidgetsFromXMI(QDomElement & qElement);
    bool loadMessagesFromXMI(QDomElement & qElement);
    bool loadAssociationsFromXMI(QDomElement & qElement);
    bool loadUisDiagramPresentation(QDomElement & qElement);

    /**
     * Contains the unique ID to allocate to a widget that needs an
     * ID for the view.  @ref ObjectWidget is an example of this.
     */
    Uml::IDType m_nLocalID;

    Uml::IDType m_nID;                ///< The ID of the view. Allocated by @ref UMLDoc.
    Uml::DiagramType m_Type;          ///< The type of diagram to represent.
    QString m_Name;                   ///< The name of the diagram.
    QString m_Documentation;          ///< The documentation of the diagram.
    Settings::OptionState m_Options;  ///< Options used by view.

    MessageWidgetList m_MessageList;  ///< All the message widgets on the diagram.
    UMLWidgetList m_WidgetList;       ///< All the UMLWidgets on the diagram.
    AssociationWidgetList m_AssociationList;  ///< All the AssociationWidgets on the diagram.

    bool m_bUseSnapToGrid;  ///< Flag to use snap to grid. The default is off.
    bool m_bUseSnapComponentSizeToGrid;  ///< Flag to use snap to grid for component size. The default is off.
    bool m_isOpen;  ///< Flag is set to true when diagram is open, i.e. shown to the user.

    // End of methods and members related to loading/saving
    ////////////////////////////////////////////////////////////////////////

    void dragEnterEvent(UMLSceneDragDropEvent* enterEvent);
    void dragMoveEvent(UMLSceneDragDropEvent* moveEvent);
    void dropEvent(UMLSceneDragDropEvent* dropEvent);

    void mouseMoveEvent(UMLSceneMouseEvent* mouseEvent);
    void mousePressEvent(UMLSceneMouseEvent* mouseEvent);
    void mouseDoubleClickEvent(UMLSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(UMLSceneMouseEvent* mouseEvent);
    virtual void contextMenuEvent(UMLSceneContextMenuEvent* contextMenuEvent);

    UMLSceneRect diagramRect();

    void makeSelected(UMLWidget* uw);

    void updateComponentSizes();

    void findMaxBoundingRectangle(const FloatingTextWidget* ft,
                                  UMLSceneValue& px, UMLSceneValue& py, UMLSceneValue& qx, UMLSceneValue& qy);
    void forceUpdateWidgetFontMetrics(QPainter *painter);

    virtual void drawBackground(QPainter *painter, const QRectF &rect);

    int m_nCollaborationId;  ///< Used for creating unique name of collaboration messages.
    UMLScenePoint m_Pos;
    bool m_bCreateObject;
    bool m_bDrawSelectedOnly;
    bool m_bPaste;
    ListPopupMenu * m_pMenu;
    bool m_bStartedCut;  ///< Flag if view/children started cut operation.
    UMLWidgetList m_selectedList;

private:
    static const UMLSceneValue defaultCanvasSize;  ///< The default size of a diagram in pixels.

    UMLView *m_view;   ///< The view to which this scene is related.
    UMLFolder *m_pFolder;  ///< The folder in which this UMLView is contained.

    ToolBarStateFactory* m_pToolBarStateFactory;
    ToolBarState* m_pToolBarState;
    IDChangeLog * m_pIDChangesLog;  ///< LocalID Changes Log for paste actions
    bool m_isActivated;             ///< True if the view was activated after the serialization(load).
    bool m_bPopupShowing;           ///< Status of a popupmenu on view. True - a popup is on view.
    UMLScenePoint m_PastePoint;     ///< The offset at which to paste the clipboard.
    UMLDoc* m_doc;                  ///< Pointer to the UMLDoc.
    UMLViewImageExporter* m_pImageExporter;  ///< Used to export the view.
    LayoutGrid*  m_layoutGrid;      ///< layout grid in the background

    void createAutoAttributeAssociation(UMLClassifier *type,
                                        UMLAttribute *attr,
                                        UMLWidget *widget);
    void createAutoConstraintAssociation(UMLEntity* refEntity,
                                         UMLForeignKeyConstraint* fkConstraint,
                                         UMLWidget* widget);

    bool isWidgetOrAssociation(const UMLScenePoint& atPos);

public slots:
    void slotToolBarChanged(int c);
    void slotObjectCreated(UMLObject * o);
    void slotObjectRemoved(UMLObject * o);
    void slotMenuSelection(QAction* action);
    void slotRemovePopupMenu();
    void slotActivate();
    void slotCutSuccessful();
    void slotShowView();

    void alignLeft();
    void alignRight();
    void alignTop();
    void alignBottom();
    void alignVerticalMiddle();
    void alignHorizontalMiddle();
    void alignVerticalDistribute();
    void alignHorizontalDistribute();

signals:
    void sigResetToolBar();

    void sigFillColorChanged(Uml::IDType);
    void sigGridColorChanged(Uml::IDType);
    void sigLineColorChanged(Uml::IDType);
    void sigTextColorChanged(Uml::IDType);
    void sigLineWidthChanged(Uml::IDType);
    void sigRemovePopupMenu();
    void sigClearAllSelected();
    void sigSnapToGridToggled(bool);
    void sigSnapComponentSizeToGridToggled(bool);
    void sigShowGridToggled(bool);
    void sigAssociationRemoved(AssociationWidget*);
    void sigWidgetRemoved(UMLWidget*);
};

QDebug operator<<(QDebug dbg, UMLScene *item);

#endif // UMLSCENE_H
