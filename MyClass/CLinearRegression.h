#pragma once
#include "dutWrapper.h"
#include "..\CommFunc.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

typedef struct PREDICTPPM
{
    int xtal_dec_val;
    double  PPM;
};

//point
class LinearRegressionPoint
{
    public:
        LinearRegressionPoint()
        {
            m_x = 0.0;
            m_y = 0.0;
        }
 
        LinearRegressionPoint(double x, double y)
        {
            m_x = x;
            m_y = y;
        }
 
        double GetX() const
        {
            return m_x;
        }
 
        double GetY() const
        {
            return m_y;
        }
    public:
        double m_x;
        double m_y;
};

class CLinearRegression
{
public:
    CLinearRegression() {};
    ~CLinearRegression() {};
        // 第一步驟：初始化
    void bubble_sort(vector<PREDICTPPM> array, int n);
    int sequentialSearch(vector<PREDICTPPM> array, int TargetPPM, int tolerance);
    int Init(const vector<LinearRegressionPoint>& points);
    double Run(double var);

private:
    vector<LinearRegressionPoint> m_points;
    /*        // 先將a和b取個隨機的初值，此處取了0
            m_a = 0;
            m_b = 0;
            double minCost = CaculateCost(m_a,m_b);
 
            double curCost = 0.0;
            // 先計算最優的a
            for(double a=MIN_a; a <= MAX_a; a+=INC)
            {
                curCost = CaculateCost(a,m_b);
                if(curCost< minCost)
                {
                    m_a = a;
                    minCost = curCost;
                }
            }
 
            // 再計算最優的b
            for(double b=MIN_b; b< =MAX_b; b+=INC)
            {
                curCost = CaculateCost(m_a,b);
                if(curCost< minCost)
                {
                    m_b = b;
                    minCost = curCost;
                }
            }
        }
        */
 
        // 第三步驟：輸出結果
 /*       int PrintResult()
        {
            printf("Y=f(X)=%lfX+(%lf)\n",m_a,m_b);
            printf("minCost=%lf\n",CaculateCost(m_a,m_b));
        }
 */
/*private:
        // 內部函式：給定a,b，輸出當前所有樣本的預計與實際值的方差
        double CaculateCost(double a, double b)
        {
            double cost = 0.0;
            double xReal = 0.0;
            double yReal = 0.0;
            double yPredict = 0.0;
            double yDef = 0.0;
            for(uint32_t i=0;i< m_points.size();++i)
            {
                // x實際值
                xReal = m_points[i].GetX();
                // y實際值
                yReal = m_points[i].GetY();
                // y預測值
                yPredict = a*xReal + b;
 
                yDef = yPredict - yReal;
                // 累加方差
                cost += (yDef*yDef);
            }
            return cost;
        }
 */

};