#include "..\stdafx.h"
#include "CLinearRegression.h"

void CLinearRegression::bubble_sort(vector<PREDICTPPM> array, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (array[j].PPM > array[j + 1].PPM)
            {
                double tempPPM = array[j].PPM;
                int tempXtal_dec_val = array[j].xtal_dec_val;
                array[j] = array[j + 1];
                array[j + 1].PPM = tempPPM;
                array[j + 1].xtal_dec_val = tempXtal_dec_val;
            }
        }
    }
}

int CLinearRegression::sequentialSearch(vector<PREDICTPPM> array, int TargetPPM, int tolerance)
{
    for (int i = 0; i < array.size(); i++)
    {
        if ((array.at(i).PPM > (TargetPPM - tolerance)) && (array.at(i).PPM < (TargetPPM + tolerance)))
        {
            return i;
        }
    }
    return -1;
}

int CLinearRegression::Init(const vector<LinearRegressionPoint>& points)
{
    if (points.size() == 0)
    {
        return -1;
    }
    m_points = points;
}

double CLinearRegression::Run(double var)
{
    double yp, y0, y1, x1, x0, x;
    x = var;
    x0 = m_points.at(0).GetX();
    x1 = m_points.at(1).GetX();
    y0 = m_points.at(0).GetY();
    y1 = m_points.at(1).GetY();
    
	yp = y0 + ((y1 - y0) / (x1 - x0)) * (x - x0);
    return yp;
}