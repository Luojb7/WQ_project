# coding: utf-8
import numpy as np
import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt

class IndicatorGallexy:
    def __init__(self, data):
        self.data = data
        self.col_head = data.columns.values.tolist()

    def isValidDate(self, str):
        try:
            dt.datetime.strptime(str, "%Y-%m-%d")
            return True
        except:
            return False
    
    def isValidTime(self, str):
        try:
            dt.datetime.strptime(str, "%Y-%m-%d %H:%M:%S")
            return True
        except:
            return False

    def dateAccess(self, date_col_head='date'):
        """判断日期格式是否能够处理，并采取对应处理措施"""
        if date_col_head not in self.col_head:
            print date_col_head+" is no exit" 
            return None
        else:
            self.data = self.data.sort_values(by=date_col_head, ascending=True)
            self.data = self.data.reset_index(drop=True)
            year_col_head = 'year'  #dataframe列名
            month_col_head = 'month'
            day_col_head = 'day'
            hour_col_head = 'hour'
            minute_col_head = 'minute'
            second_col_head = 'second'

            date = []
            if self.isValidDate(self.data[date_col_head][0]): 
                for i in range(len(self.data[date_col_head])):
                    date.append(dt.datetime.strptime(self.data[date_col_head][i], "%Y-%m-%d"))
                date_list = []
                for d in date:
                    temp = []
                    temp.append(d.year)
                    temp.append(d.month)
                    temp.append(d.day)
                    date_list.append(temp)
                del self.data[date_col_head]
                date_frame = pd.DataFrame(date_list, columns=[year_col_head, month_col_head, day_col_head])
                self.data = pd.concat([date_frame, self.data], axis=1)
                self.col_head = self.data.columns.values.tolist()

            elif self.isValidTime(self.data[date_col_head][0]):  
                for i in range(len(self.data[date_col_head])):
                    date.append(dt.datetime.strptime(self.data[date_col_head][i], "%Y-%m-%d %H:%M:%S"))
                date_list = []
                for d in date:
                    temp = []
                    temp.append(d.year)
                    temp.append(d.month)
                    temp.append(d.day)
                    temp.append(d.hour)
                    temp.append(d.minute)
                    temp.append(d.second)
                    date_list.append(temp)
                del self.data[date_col_head]
                date_frame = pd.DataFrame(date_list, columns=[year_col_head, month_col_head, day_col_head, hour_col_head, minute_col_head, second_col_head])
                self.data = pd.concat([date_frame, self.data], axis=1)
                self.col_head = self.data.columns.values.tolist()

            else:
                print "the data format can't be processed"
                return None
    
    def getAtt(self, s_att):
        """返回某一属性列值"""
        s_att = self.data[s_att]
        return s_att

    def getEMA(self, col='close', ema_model='ema12', ema_molecule=11, ema_denominator=13, init_ema=0):
        """
            EMA(12) = EMA(12) * 11/13 + close * 2/13
            EMA(26) = EMA(26) * 25/27 + close * 2/27
        """
        ema_model = ema_model+'_'+col 
        fac1 = ema_molecule / ema_denominator
        fac2 = 1 - fac1

        ema = []
        ema.append(init_ema * fac1 + self.data[col][0] * fac2)
        for i in range(1, len(self.data[col])):
            ema.append(ema[i-1] * fac1 + self.data[col][i] * fac2)

        ema = pd.DataFrame(ema, columns=[ema_model])
        return ema

    def getDIFF(self, first_ema, second_ema, col='close'):
        """  DIFF = EMA(12) - EMA(26) """
        first_ema_col_head = first_ema.columns.values.tolist()
        second_ema_col_head = second_ema.columns.values.tolist()
        diff_col_head = 'diff_'+col

        diff = []
        for i in range(len(first_ema)):
            temp = first_ema[first_ema_col_head[0]][i] - second_ema[second_ema_col_head[0]][i]
            diff.append(temp)
        diff = pd.DataFrame(diff, columns=[diff_col_head])
        return diff

    def getDEA(self, diff, dea_molecule=8, dea_denominator=10, init_dea=0, col='close'):
        """ DEA = DEA * 8/10 + DIF * 2/10 """
        diff_col_head = diff.columns.values.tolist()
        dea_head = 'dea_'+col
        fac1 = dea_molecule / dea_denominator
        fac2 = 1 - fac1

        dea = []
        dea.append(init_dea * fac1 + diff[diff_col_head[0]][0] * fac2)
    
        for i in range(1,len(diff)):
            dea.append(dea[i-1] * fac1 + diff[diff_col_head[0]][i] * fac2)
        
        dea = pd.DataFrame(dea, columns=[dea_head])
        return dea

    def getMACD(self, diff, dea, col='close'):
        """ macd = 2 * (DIFF - DEA) """
        diff_col_head = diff.columns.values.tolist()
        dea_col_head = dea.columns.values.tolist()
        macd_col_head = 'macd_'+col

        macd = []
        for i in range(len(diff)):
            macd.append(2 * (diff[diff_col_head[0]][i] - dea[dea_col_head[0]][i]))
        
        macd = pd.DataFrame(macd, columns=[macd_col_head])
        return macd

    def getBOLL(self):
        """compute boll"""
        pass

    def getRSI(self, selected_att='price_change', power=100):
        """RSI(N) = A / (A+B) * 100"""
        posi = 0
        nega = 0
        colhead = 'rsi_'+selected_att

        rsi = []
        length = len(self.data[selected_att]) - 5
        for i in range(length):
            for j in range(5):
                if self.data[selected_att][i+j] > 0:
                    posi += self.data[selected_att][i+j]
                else:
                    nega += self.data[selected_att][i+j]
            rsi.append(posi * (posi - nega ) * power)
        for i in range(5):
            rsi.append(rsi[length])

        rsi = pd.DataFrame(rsi, columns=[colhead])
        return rsi

    def mergeData(self, add_att_df):
        self.data = pd.concat([self.data, add_att_df], axis=1)

    def getColHead(self):
        self.col_head = self.data.columns.values.tolist()
        return self.col_head

    def getData(self):
        return self.data

    def plotData(self, col_list):
        """根据输入的属性列表，画出对比图像"""
        temp_list = []
        for cl in col_list:
            if cl not in self.col_head:
                print cl+' not in the data set'
            else:
                temp_list.append(self.data[cl])
        
        temp_df = pd.concat(temp_list, axis=1)
        temp_df.plot()
        plt.show()
        

if __name__ == "__main__":
    data = pd.read_csv("data/1day/000001.csv")
    
    xmIG = IndicatorGallexy(data)

    xmIG.dateAccess('date')

    ema12 = xmIG.getEMA()
    ema26 = xmIG.getEMA('close', 'ema26', 25, 27, 0)
    xmIG.mergeData(ema12)
    xmIG.mergeData(ema26)

    diff = xmIG.getDIFF(ema12, ema26)
    xmIG.mergeData(diff)

    dea = xmIG.getDEA(diff, 8, 10, 0)
    xmIG.mergeData(dea)

    macd = xmIG.getMACD(diff, dea)
    xmIG.mergeData(macd)

    col_list = ['open', 'close']
    xmIG.plotData(col_list)