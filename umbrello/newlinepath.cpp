/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "newlinepath.h"

// qt includes
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>

#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QtXml/QDomNodeList>

namespace New
{
    /**
     * @class HeadSymbol
     *
     * This class provides with various symbols that can be used as
     * "head" at any end of the LinePath.  It also provides with
     * convenience methods to align the head to LinePath.
     */

    HeadSymbol::SymbolProperty HeadSymbol::symbolTable[Count] =
    {
        {
            QRectF(-6, 0, 12, 10), QPainterPath(), QLineF(0, 0, 0, 10),
            PointPair(QPointF(0, 10), QPointF(0, 10))
        },
        {
            QRectF(-5, -10, 10, 20), QPainterPath(), QLineF(0, -10, 0, 10),
            PointPair(QPointF(0, -10), QPointF(0, 10))
        },
        {
            QRectF(-8, -8, 16, 16), QPainterPath(), QLineF(0, -8, 0, 8),
            PointPair(QPointF(0, -8), QPointF(0, 8))
        }

    };

    /// @internal A convenience method to setup shapes of all symbols.
    void setupSymbolTable()
    {
        HeadSymbol::SymbolProperty &arrow = HeadSymbol::symbolTable[HeadSymbol::Arrow];
        if (arrow.shape.isEmpty()) {
            QRectF rect = arrow.boundRect;
            // Defines a 'V' shape arrow fitting in the bound rect.
            arrow.shape.moveTo(rect.topLeft());
            arrow.shape.lineTo(rect.center().x(), rect.bottom());
            arrow.shape.lineTo(rect.topRight());
        }

        HeadSymbol::SymbolProperty &diamond = HeadSymbol::symbolTable[HeadSymbol::Diamond];
        if (diamond.shape.isEmpty()) {
            QRectF rect = diamond.boundRect;
            // Defines a 'diamond' shape fitting in the bound rect.
            diamond.shape.moveTo(rect.center().x(), rect.top());
            diamond.shape.lineTo(rect.left(), rect.center().y());
            diamond.shape.lineTo(rect.center().x(), rect.bottom());
            diamond.shape.lineTo(rect.right(), rect.center().y());
            diamond.shape.lineTo(rect.center().x(), rect.top());
        }

        HeadSymbol::SymbolProperty &circle = HeadSymbol::symbolTable[HeadSymbol::Circle];
        if (circle.shape.isEmpty()) {
            QRectF rect = circle.boundRect;
            // Defines a circle with a horizontal-vertical cross lines.
            circle.shape.addEllipse(rect);

            circle.shape.moveTo(rect.center().x(), rect.top());
            circle.shape.lineTo(rect.center().x(), rect.bottom());

            circle.shape.moveTo(rect.left(), rect.center().y());
            circle.shape.lineTo(rect.right(), rect.center().y());
        }

    }


    /**
     * Constructs a HeadSymbol with current symbol being \a symbol and
     * parented to \a parent.
     */
    HeadSymbol::HeadSymbol(Symbol symbol, QGraphicsItem *parent) :
        QGraphicsItem(parent),
        m_firstTime(true)
    {
        // Ensure SymbolTable is validly initialized.
        setupSymbolTable();
        setSymbol(symbol);
    }

    /// Destructor
    HeadSymbol::~HeadSymbol()
    {
    }

    /// @return The current symbol being represented.
    HeadSymbol::Symbol HeadSymbol::symbol() const
    {
        return m_symbol;
    }

    /// Sets the current symbol type to \a symbol and updates the geometry.
    void HeadSymbol::setSymbol(Symbol symbol)
    {
        if (m_firstTime) {
            m_firstTime = false;
            // dont crash during this func call in constructor.
        }
        else {
            prepareGeometryChange(); //calls update implicitly
        }
        m_symbol = symbol;
    }

    /// Draws the current symbol using the QPainterPath stored for the current symbol.
    void HeadSymbol::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
    {
        painter->setPen(m_pen);
        painter->setBrush(m_brush);
        painter->drawPath(HeadSymbol::symbolTable[m_symbol].shape);
    }

    /// @return The bound rectangle for this based on current symbol.
    QRectF HeadSymbol::boundingRect() const
    {
        const qreal adj = .5 * m_pen.widthF();
        return HeadSymbol::symbolTable[m_symbol].boundRect.
            adjusted(-adj, -adj, adj, adj);
    }

    /// @return The path for this based on current symbol.
    QPainterPath HeadSymbol::shape() const
    {
        QPainterPath path;
        path.addRect(boundingRect());
        return path;
    }

    /**
     * This method aligns *this* HeadSymbol to the line being
     * passed. That is, it ensures that the axis of this symbol aligns
     * exactly with the \a "to" line passed.
     *
     * Also this item is moved such that the second end point of the
     * PointPair for the current symbol *collides* with the second end
     * point of \a "to" line.
     */
    void HeadSymbol::alignTo(const QLineF& to)
    {
        QLineF toMapped(mapFromParent(to.p1()), mapFromParent(to.p2()));

        QLineF origAxis = axisLine();
        QLineF translatedAxis = origAxis.translated(toMapped.p2() - origAxis.p2());

        qreal angle = translatedAxis.angleTo(toMapped);
        rotate(-angle);

        QPointF delta = to.p2() - mapToParent(symbolEndPoints().second);
        moveBy(delta.x(), delta.y());
    }

    /// @return The axis line for this item based on current symbol.
    QLineF HeadSymbol::axisLine() const
    {
        return HeadSymbol::symbolTable[m_symbol].axisLine;
    }

    /**
     * @return The end points for the symbol.
     *
     *         The first point is where the LinePath's visible line is
     *         supposed to end.
     *
     *         The second points is where the actual "head" part is to
     *         appear.
     */
    PointPair HeadSymbol::symbolEndPoints() const
    {
        return HeadSymbol::symbolTable[m_symbol].endPoints;
    }

    /// @return The pen used to draw symbol.
    QPen HeadSymbol::pen() const
    {
        return m_pen;
    }

    /// Sets the pen used to draw the symbol
    void HeadSymbol::setPen(const QPen& pen)
    {
        prepareGeometryChange();
        m_pen = pen;
    }

    /// @return The brush used to fill symbol.
    QBrush HeadSymbol::brush() const
    {
        return m_brush;
    }

    /// Sets the brush used to fill symbol.
    void HeadSymbol::setBrush(const QBrush &brush)
    {
        m_brush = brush;
        update();
    }


    /**
     * @class LinePath
     *
     * @short A class to manage a set of connected lines (eg Association line).
     *
     * This class inherits QGraphicsItem and hence can be directly
     * embedded in other QGraphicsItem. It provides interface methods
     * to insert, remove ..  Also it handles mouse events to allow
     * user to insert line points, move line points and also move
     * lines.
     *
     * @author Gopala Krishna
     * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
     */


    // Initialize static variables.
    const qreal LinePath::Delta = 5;
    const qreal LinePath::SelectedPointDiameter = 8;


    /**
     * Constructs a LinePath item with its parent being \a parent.
     */
    LinePath::LinePath(QGraphicsItem *parent) : QGraphicsItem(parent)
    {
        m_activePointIndex = m_activeSegmentIndex = -1;
        m_startHeadSymbol = m_endHeadSymbol = 0;
        setFlags(ItemIsSelectable | ItemIsFocusable);
    }

    /// Destructor
    LinePath::~LinePath()
    {
    }

    /// @return The point at given index.
    QPointF LinePath::point(int index) const
    {
        // QVector::at asserts for index validity.
        return m_points.at(index);
    }

    /**
     * Sets the point value at given index to \a point.  Also changes
     * the bounding rect appropriately.
     */
    void LinePath::setPoint(int index, const QPointF& point)
    {
        if (m_points.at(index) == point) {
            return; // Nothing to change
        }
        prepareGeometryChange();
        m_points[index] = point;
        calculateBoundingRect();
    }

    /**
     * Inserts the passed in \a point at the \a index passed in and
     * recalculates the bounding rect.
     */
    void LinePath::insertPoint(int index, const QPointF& point)
    {
        prepareGeometryChange();
        m_points.insert(index, point);
        calculateBoundingRect();
    }

    /**
     * Removes the point at \a index passed in.
     * @see removeNonEndPoint
     */
    void LinePath::removePoint(int index)
    {
        prepareGeometryChange();
        m_points.remove(index);
        calculateBoundingRect();
    }

    /**
     * This method ensures the point at \a index being removed is
     * neither the start nor the end point.
     * @see removeNonEndPoint
     */
    void LinePath::removeNonEndPoint(int index)
    {
        if (index == 0 || index == (m_points.size() - 1)) {
            uDebug() << "Invalid index : " << index;
            return;
        }
        removePoint(index);
    }

    /**
     * Clears the linepath by removing all the points in the linepath.
     */
    void LinePath::clear()
    {
        if (!m_points.isEmpty()) {
            prepareGeometryChange();
            m_points.clear();
            calculateBoundingRect();
        }
    }

    /**
     * This method removes duplicate coniguous points.
     */
    void LinePath::removeDuplicatePoints()
    {
        int i = 1;
        while (i < m_points.size()) {
            if (m_points.at(i) == m_points.at(i-1)) {
                m_points.remove(i);
            }
            else {
                ++i;
            }
        }
    }

    /**
     * This method optimizes the number of points in the
     * LinePath. This can be used to reduce the clutter caused due to
     * too many points.
     */
    void LinePath::optimizeLinePoints()
    {
        //TODO: Implement this.
    }

    /**
     * @retval "Index" of the linepoint closest to the \a point
     *         passed.
     * @rertval -1 If no linepoint is closer to passed in \a point.
     *
     * @param point The point which is to be tested for closeness.
     * @param delta The closeness is measured by "delta" which indicates radius
     *              around the linepoint to be regarded as closer.
     *
     * @note The end points aren't checked as they aren't drawn anyway
     *       and hence they can never be active.
     */
    int LinePath::closestPointIndex(const QPointF& point, qreal delta) const
    {
        const int sz = m_points.size();
        // Don't try for end points as they aren't drawn
        for(int i = 1; i < sz - 1; ++i) {
            const QPointF& linePoint = m_points.at(i);
            // Apply distance formula to see point closeness.
            qreal deltaXSquare = (point.x() - linePoint.x()) * (point.x() - linePoint.x());
            qreal deltaYSquare = (point.y() - linePoint.y()) * (point.y() - linePoint.y());

            qreal lhs = deltaXSquare + deltaYSquare;
            qreal rhs = delta * delta;

            if (lhs <= rhs) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @retval "Index" of the segment on which the point lies (fuzzy)
     * @retval -1 If no segment contains the point
     *
     * @param point The point to be tested for line-segment closure.
     * @param delta Represents extent of fuzzy region around the line
     *              to be tested for point closure.
     */
    int LinePath::segmentIndex(const QPointF& point, qreal delta) const
    {
        QPainterPathStroker stroker;
        stroker.setWidth(delta);

        for(int i = 1; i < m_points.size(); ++i) {
            QLineF segment(m_points[i-1], m_points[i]);

            QPainterPath path;
            path.moveTo(segment.p1());
            path.lineTo(segment.p2());

            path = stroker.createStroke(path);

            if (path.contains(point)) {
                return i-1;
            }
        }

        return -1;
    }

    /**
     * @retval True If point is either the start or end of linepath.
     */
    bool LinePath::isEndPoint(const QPointF& point) const
    {
        const int sz = m_points.size();
        if (sz == 0) {
            return false;
        }
        return m_points[0] == point || m_points[sz-1] == point;
    }

    /**
     * retval True If point at \a index is start or end.
     */
    bool LinePath::isEndPointIndex(int index) const
    {
        const int sz = m_points.size();
        return (sz != 0) && (index == 0 || index == (sz - 1));
    }

    /**
     * Sets the start and end points to passed parameters.
     */
    void LinePath::setEndPoints(const QPointF& start, const QPointF& end)
    {
        int size = m_points.size();

        prepareGeometryChange();

        if (size == 0) {
            m_points.insert(0, start);
            m_points.insert(1, end);
        }
        else if (size == 1) {
            m_points[0] = start;
            m_points.insert(1, end);
        }
        else {
            m_points[0] = start;
            m_points[1] = end;
        }

        calculateBoundingRect();
    }

    /**
     * Sets the HeadSymbol to appear at the first line segment to \a
     * symbol.
     *
     * If symbol == HeadSymbol::None , then it deletes the symbol item.
     *
     * Also this method aligns the head symbols.
     */
    void LinePath::setStartHeadSymbol(HeadSymbol::Symbol symbol)
    {
        Q_ASSERT(symbol != HeadSymbol::Count);
        if (symbol == HeadSymbol::None) {
            delete m_startHeadSymbol;
            m_startHeadSymbol = 0;
            return;
        }

        if (m_startHeadSymbol) {
            m_startHeadSymbol->setSymbol(symbol);
        }
        else {
            m_startHeadSymbol = new HeadSymbol(symbol, this);
        }
        m_startHeadSymbol->setPen(m_pen);
        alignHeadSymbols();
    }

    /**
     * Sets the HeadSymbol to appear at the last line segment to \a
     * symbol.
     *
     * If symbol == HeadSymbol::None , then it deletes the symbol item.
     *
     * Also this method aligns the head symbols.
     */
    void LinePath::setEndHeadSymbol(HeadSymbol::Symbol symbol)
    {
        Q_ASSERT(symbol != HeadSymbol::Count);
        if (symbol == HeadSymbol::None) {
            delete m_endHeadSymbol;
            m_endHeadSymbol = 0;
            return;
        }

        if (m_endHeadSymbol) {
            m_endHeadSymbol->setSymbol(symbol);
        }
        else {
            m_endHeadSymbol = new HeadSymbol(symbol, this);
        }
        m_endHeadSymbol->setPen(m_pen);
        alignHeadSymbols();
    }

    /**
     * This method aligns both the \b "start" and \b "end" symbols to
     * the current angles of the \b "first" and the \b "last" line
     * segment respectively.
     */
    void LinePath::alignHeadSymbols()
    {
        const int sz = m_points.size();
        if (sz < 2) {
            // Cannot align if there is no line (one line = 2 points)
            return;
        }

        if (m_startHeadSymbol) {
            QLineF segment(m_points[1], m_points[0]);
            m_startHeadSymbol->alignTo(segment);
        }

        if (m_endHeadSymbol) {
            QLineF segment(m_points[sz-2], m_points[sz - 1]);
            m_endHeadSymbol->alignTo(segment);
        }
    }

    /**
     * @return The number of points in the linepath.
     */
    int LinePath::count() const
    {
        return m_points.size();
    }

    /**
     * @return An index at which the passed \a point can be inserted
     *         provided the \a point lies on one of line of this
     *         linepath.
     */
    int LinePath::insertableLinePathIndex(const QPointF &pos) const
    {
        QPainterPathStroker stroker;
        stroker.setWidth(LinePath::Delta);

        for(int i = 1; i < m_points.size(); ++i) {
            QLineF segment(m_points[i-1], m_points[i]);

            QPainterPath path;
            path.moveTo(segment.p1());
            path.lineTo(segment.p2());

            path = stroker.createStroke(path);

            if (path.contains(pos)) {
                return i;
            }
        }

        return -1;
    }

    /**
     * Loads LinePath information saved in \a qElement XMI element.
     */
    bool LinePath::loadFromXMI(QDomElement &qElement)
    {
        if (!qElement.hasChildNodes()) {
            return false;
        }

        QDomNodeList domNodes = qElement.childNodes();
        for (int i = 0; i < domNodes.size(); ++i) {
            QDomElement ele = domNodes.at(i).toElement();
            qreal x = ele.attribute("x", "0").toDouble();
            qreal y = ele.attribute("y", "0").toDouble();
            insertPoint(i, QPointF(x, y));
            ++i;
        }

        return true;
    }

    /**
     * Saves linepath information into XMI element named "linepath".
     */
    void LinePath::saveToXMI(QDomDocument &qDoc, QDomElement &qElement)
    {
        QDomElement pathElement = qDoc.createElement("linepath");
        for (int i = 0; i < m_points.size(); ++i ) {
            QDomElement pointElement = qDoc.createElement("point");
            const QPointF &point = m_points.at(i);
            pointElement.setAttribute( "x", point.x() );
            pointElement.setAttribute( "y", point.y() );
            pathElement.appendChild(pointElement);
        }
        qElement.appendChild(pathElement);
    }

    /// @return The "pen" used to draw the lines of this linepath.
    QPen LinePath::pen() const
    {
        return m_pen;
    }

    /**
     * Set the "pen" used to draw the lines of this linepath.
     */
    void LinePath::setPen(const QPen& pen)
    {
        prepareGeometryChange();
        m_pen = pen;
        if (m_startHeadSymbol) {
            m_startHeadSymbol->setPen(m_pen);
        }
        if (m_endHeadSymbol) {
            m_endHeadSymbol->setPen(m_pen);
        }
        calculateBoundingRect();
    }

    /// @return Color of line being drawn.
    QColor LinePath::lineColor() const
    {
        return m_pen.color();
    }

    /// Sets the color for the lines being drawn.
    void LinePath::setLineColor(const QColor& color)
    {
        m_pen.setColor(color);
        if (m_startHeadSymbol) {
            m_startHeadSymbol->setPen(m_pen);
        }
        if (m_endHeadSymbol) {
            m_endHeadSymbol->setPen(m_pen);
        }
        update();
    }

    /// @return The width of the line being drawn.
    uint LinePath::lineWidth() const
    {
        return m_pen.width();
    }

    /// Sets the width of line being drawn to \a width.
    void LinePath::setLineWidth(uint width)
    {
        prepareGeometryChange();
        m_pen.setWidth(width);
        if (m_startHeadSymbol) {
            m_startHeadSymbol->setPen(m_pen);
        }
        if (m_endHeadSymbol) {
            m_endHeadSymbol->setPen(m_pen);
        }
        calculateBoundingRect();
    }

    /// @return The bounding rectangle for the linepath.
    QRectF LinePath::boundingRect() const
    {
        return m_boundingRect;
    }

    /// @return The shape of the linepath.
    QPainterPath LinePath::shape() const
    {
        return m_shape;
    }

    /// Draws the line path and also takes care of highlighting active point or line.
    void LinePath::paint(QPainter *painter, const QStyleOptionGraphicsItem *opt, QWidget *)
    {
        painter->setPen(m_pen);
        painter->setBrush(Qt::NoBrush);

        int sz = m_points.size();
        if (sz == 0) {
            return;
        }

        QPointF savedStart = m_points.first();
        QPointF savedEnd = m_points.last();

        // Modify the m_points array not to include the HeadSymbol, the value depends on HeadSymbol.
        if (m_startHeadSymbol) {
            QPointF newStart = m_startHeadSymbol->mapToParent(m_startHeadSymbol->symbolEndPoints().first);
            m_points[0] = newStart;
        }

        if (m_endHeadSymbol) {
            QPointF newEnd = m_endHeadSymbol->mapToParent(m_endHeadSymbol->symbolEndPoints().first);
            m_points[sz - 1] = newEnd;
        }

        painter->drawPolyline(m_points.constData(), m_points.size());

        // Now restore the points array
        m_points[0] = savedStart;
        m_points[sz - 1] = savedEnd;

        if (opt->state & QStyle::State_Selected) {
            QRectF ellipse(0, 0, SelectedPointDiameter, SelectedPointDiameter);
            painter->setBrush(Qt::blue);
            for (int i = 1; i < sz - 1; ++i) {
                ellipse.moveCenter(m_points.at(i));
                painter->drawEllipse(ellipse);
            }

            if (m_activePointIndex != -1) {
                ellipse.moveCenter(m_points.at(m_activePointIndex));
                painter->setBrush(Qt::darkBlue);
                painter->drawEllipse(ellipse);
            }
            else if (m_activeSegmentIndex != -1) {
                painter->setPen(Qt::yellow);
                painter->drawLine(QLineF(m_points[m_activeSegmentIndex], m_points[m_activeSegmentIndex+1]));
            }
        }
    }

    /// Determines the active point or segment, the latter being given more priority.
    void LinePath::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            m_activePointIndex = closestPointIndex(event->pos());
            // calculate only if active point index is -1
            m_activeSegmentIndex = (m_activePointIndex != -1) ? -1 : segmentIndex(event->pos());
        }
        else {
            m_activePointIndex = m_activeSegmentIndex = -1;
        }
        QGraphicsItem::mousePressEvent(event);
    }

    /// Moves the point or line if active.
    void LinePath::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        if (m_activePointIndex != -1) {
            setPoint(m_activePointIndex, event->pos());
        }
        else if (m_activeSegmentIndex != -1) {
            QPointF delta = event->scenePos() - event->lastScenePos();
            setPoint(m_activeSegmentIndex, m_points[m_activeSegmentIndex] + delta);
            setPoint(m_activeSegmentIndex + 1, m_points[m_activeSegmentIndex + 1] + delta);
        }
    }

    /// Reset active indices and also push undo command.
    void LinePath::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if (event->buttons() & Qt::LeftButton) {
            m_activeSegmentIndex = m_activePointIndex = -1;
        }
        QGraphicsItem::mouseReleaseEvent(event);
    }

    /// Inserts a new point at double click position.
    void LinePath::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
    {
        int index = closestPointIndex(event->pos());
        // First check if double click was on a point
        if (index != -1) {
            // If on point but not endpoint, delete the point
            if (!isEndPointIndex(index)) {
                removePoint(index);
            }
        }
        else {
            // Else insert a new point on the line segment
            index = insertableLinePathIndex(event->pos());
            if (index != -1) {
                insertPoint(index, event->pos());
            }
        }
    }

    /**
     * Calculates the "to be highlighted" point and segment indicies
     * and updates if necessary.
     */
    void LinePath::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
    {
        int oldPointIndex = m_activePointIndex;
        int oldSegmentIndex = m_activeSegmentIndex;

        m_activePointIndex = closestPointIndex(event->pos());
        // Activate segment index only if point index is -1
        m_activeSegmentIndex = (m_activePointIndex != -1) ? -1 : segmentIndex(event->pos());

        bool isChanged = (oldSegmentIndex != m_activeSegmentIndex || oldPointIndex != m_activePointIndex);
        if (isChanged) {
            update();
        }
    }

    /**
     * Calculates the "to be highlighted" point and segment indicies
     * and updates if necessary.
     */
    void LinePath::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
    {
        int oldPointIndex = m_activePointIndex;
        int oldSegmentIndex = m_activeSegmentIndex;

        m_activePointIndex = closestPointIndex(event->pos());
        // Activate segment index only if point index is -1
        m_activeSegmentIndex = (m_activePointIndex != -1) ? -1 : segmentIndex(event->pos());

        bool isChanged = (oldSegmentIndex != m_activeSegmentIndex || oldPointIndex != m_activePointIndex);
        if (isChanged) {
            update();
        }
    }

    /// Reset active indicies and updates.
    void LinePath::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
    {
        m_activePointIndex = m_activeSegmentIndex = -1;
        update();
    }

    /**
     * Enables hover events on selection and disables the same on
     * deselection.
     */
    QVariant LinePath::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        if (change == ItemSelectedHasChanged) {
            setAcceptHoverEvents(value.toBool());
        }
        return QGraphicsItem::itemChange(change, value);
    }

    /**
     * Calculates the "shape" and also the "bounding rectangle"
     * required by GraphicsView framework.
     */
    void LinePath::calculateBoundingRect()
    {
        if (m_points.isEmpty()) {
            m_shape = QPainterPath();
            m_boundingRect = QRectF();
            return;
        }

        QPainterPath path;
        path.moveTo(m_points.first());
        for(int i = 1; i < m_points.size(); ++i) {
            const QPointF& pt = m_points.at(i);
            path.lineTo(pt);
        }

        QRectF ellipse(0, 0, SelectedPointDiameter, SelectedPointDiameter);
        foreach(QPointF point, m_points) {
            ellipse.moveCenter(point);
            path.addEllipse(ellipse);
        }

        QPainterPathStroker stroker;
        stroker.setWidth(LinePath::Delta); // allow delta region

        m_shape = stroker.createStroke(path);
        m_boundingRect = m_shape.boundingRect();

        alignHeadSymbols();
    }
}