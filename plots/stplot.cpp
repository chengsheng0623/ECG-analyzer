#include "stplot.h"
#include <qwt_symbol.h> 
#include <qwt_legend.h> 

StPlot::StPlot(QWidget* parent): QwtPlot(parent)
{
  setMinimumHeight(10);
  setMinimumWidth(10);
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(this);
  setAxisTitle(QwtPlot::xBottom, "Czas [s]");
  setAxisTitle(QwtPlot::yLeft, "Amplituda [mv]");
  
  curve = new QwtPlotCurve("signal");
  curve->setYAxis(QwtPlot::yLeft);
  curve->attach(this);

  ISOPoints = new QwtPlotCurve("ISO");
  ISOPoints->setStyle(QwtPlotCurve::NoCurve);
  ISOPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::green), QColor(Qt::green), QSize(5, 5)));
  ISOPoints->setYAxis(QwtPlot::yLeft);
  ISOPoints->attach(this);
  
  JPoints = new QwtPlotCurve("J");
  JPoints->setStyle(QwtPlotCurve::NoCurve);
  JPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::blue), QColor(Qt::blue), QSize(5, 5)));
  JPoints->setYAxis(QwtPlot::yLeft);
  JPoints->attach(this);
  
  STPoints = new QwtPlotCurve("ST");
  STPoints->setStyle(QwtPlotCurve::NoCurve);
  STPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::red), QColor(Qt::red), QSize(5, 5)));
  STPoints->setYAxis(QwtPlot::yLeft);
  STPoints->attach(this);

  RPoints = new QwtPlotCurve("R");
  RPoints->setStyle(QwtPlotCurve::NoCurve);
  RPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::yellow), QColor(Qt::yellow), QSize(5, 5)));
  RPoints->setYAxis(QwtPlot::yLeft);
  RPoints->attach(this);

  QwtLegend * legend = new QwtLegend();
  legend->setItemMode(QwtLegend::ReadOnlyItem);
  legend->setWhatsThis("Click on an item to show/hide the plot");
  this->insertLegend(legend, QwtPlot::RightLegend);
  
  samples = new QVector<QPointF>;
  data = new QwtPointSeriesData;
  replot();
  
  zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas());
  zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect4, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect5, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect6, Qt::NoButton);
}

StPlot::~StPlot()
{
  if(zoomer) delete zoomer;
  delete ISOPoints;
  delete JPoints;
  delete STPoints;
  delete RPoints;
}

void StPlot::setSignal(const ECGSignalChannel& signal, const ECGChannelInfo& info, const ECGST& stdata)
{
  gsl_vector *v = signal->signal;
  invgain = 1.0 / float(info.gain);
  dt = 1.0 / float(info.frequecy);
  int size = int(v->size);
  samples->clear();
  for (int i = 0; i < size; i++)
    samples->push_back(QPointF(float(i)*dt, float(v->data[i*v->stride])*invgain));
  
  QVector<QPointF>* ISOVector = new QVector<QPointF>;
  QVector<QPointF>* JVector = new QVector<QPointF>;
  QVector<QPointF>* STVector = new QVector<QPointF>;
  QVector<QPointF>* RVector = new QVector<QPointF>;
  const std::vector<ECGST::Interval> intervals = stdata.getIntervals();
  for (std::vector<ECGST::Interval>::const_iterator it = intervals.begin() ; it != intervals.end(); ++it)
  {
    ISOVector->push_back(QPointF(float(it->isopoint)*dt, float(v->data[(it->isopoint)*v->stride]*invgain)));
    JVector->push_back(QPointF(float(it->jpoint)*dt, float(v->data[(it->jpoint)*v->stride]*invgain)));
    STVector->push_back(QPointF(float(it->stpoint)*dt, float(v->data[(it->stpoint)*v->stride]*invgain)));
    RVector->push_back(QPointF(float(it->rpoint)*dt, float(v->data[(it->rpoint)*v->stride]*invgain)));
  }
  
  ISOPoints->setSamples(*ISOVector);
  JPoints->setSamples(*JVector);
  STPoints->setSamples(*STVector);
  RPoints->setSamples(*RVector);
  data->setSamples(*samples);
  curve->setData(data);
  zoomer->setZoomBase();
  replot();
}



void StPlot::zoomX(int from, int to, bool vscale)
{
  QRectF rect = zoomer->zoomBase();
  
  
  rect.setLeft(from*dt);
  rect.setRight(to*dt);
  
  if (vscale) {
    auto _minmax = minMaxValueIn(from, to);
    rect.setTop(_minmax.second);
    rect.setBottom(_minmax.first);
  }
  
  zoomer->zoom(rect);
}

std::pair<double, double> StPlot::minMaxValueIn(int from, int to)
{
  from = from < 0 ? 0 : from;
  to = to > (data->size() - 1) ? data->size() : to;
  QVector<QPointF> roi(to - from);
  std::copy(samples->begin()+from, samples->begin()+to, roi.begin());
  auto _minmax = std::minmax_element(roi.begin(), roi.end(), [](const QPointF& a, const QPointF& b) {
    return a.y() < b.y();
  });
  return std::make_pair(_minmax.first->y(), _minmax.second->y());
}

