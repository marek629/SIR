#include "timecombobox.h"

TimeComboBox::TimeComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void TimeComboBox::addItem(const QString &text, const QVariant &userData)
{
    int idx = findText(text);
    if ( idx != -1 && !duplicatesEnabled() )
        setCurrentIndex(idx);
    else
    {   // add item to right index within combobox
        int i = rigthIndex(text);
        insertItem(i, text, userData);
        setCurrentIndex(i);
    }
}

int TimeComboBox::rigthIndex(const QString &text)
{
    QStringList list = text.split(' ');
    if (text.contains('/'))
    {
        if (list.length() == 2)
        {
            QStringList strFraction = list.first().split('/');
            double expTimeFraction = strFraction.first().toDouble() /
                    strFraction.last().toDouble();
            for (int i=1; ; i++)
            {
                QStringList strList = itemText(i).split(' ');
                strFraction = strList.first().split('/');
                double itemFraction = strFraction.first().toDouble() /
                        strFraction.last().toDouble();
                if (strList.length()==2 && itemFraction>expTimeFraction)
                    return i;
            }
        }
        else // expTime > 1s
        {
            for (int i=count()-1; ; i--)
            {
                QStringList strList = itemText(i).split(' ');
                if ( strList.first().toInt() <= list.first().toInt() )
                {
                    if ( strList.length() == 2 )
                        return i+1;
                    else
                    {
                        QStringList strFraction = list.at(1).split('/');
                        double expTimeFraction =
                                strFraction.first().toDouble() /
                                strFraction.at(1).toDouble();
                        strFraction = strList.at(1).split('/');
                        double itemFraction =
                                strFraction.first().toDouble() /
                                strFraction.at(1).toDouble();
                        if ( expTimeFraction == itemFraction )
                            return i;
                        else if ( expTimeFraction > itemFraction )
                            return i+1;
                        else
                            return i-1;
                    }
                }
            }
        }
    }
    else
    {
        int expTimeInt = list.first().toInt();
        for (int i=count()-1; ; i--)
        {
            int integer = itemText(i).split(' ').first().toInt();
            if ( expTimeInt > integer )
                return i+1;
        }
    }
}
