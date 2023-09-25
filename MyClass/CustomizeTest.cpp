#include "..\stdafx.h"
#include "CustomizeTest.h"

void CCustomizeTest::test()
{
    CLinearRegression m_CLinearRegression;
    
    vector<LinearRegressionPoint> points;
    vector<PREDICTPPM> PredictPPM;
    PREDICTPPM m_PREDICTPPM;

    int  MIN_LIMIT = 50, MAX_LIMIT = 255;

    points.push_back(LinearRegressionPoint(MIN_LIMIT, -30.6));
    points.push_back(LinearRegressionPoint(MAX_LIMIT, 61.3));
    m_CLinearRegression.Init(points);

    for (int i = MIN_LIMIT; i < MAX_LIMIT; i++)
    {
        m_PREDICTPPM.PPM = m_CLinearRegression.Run(i);
        m_PREDICTPPM.xtal_dec_val = i;
        PredictPPM.push_back(m_PREDICTPPM);
        printf("1111");
    }

    //bubble_sort(PredictPPM, PredictPPM.size());
    int index = m_CLinearRegression.sequentialSearch(PredictPPM, 0, 2);
    printf("1111");
}