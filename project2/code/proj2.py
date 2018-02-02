import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime as dt
from sklearn import preprocessing
from sklearn.neural_network import MLPRegressor
from sklearn import tree
from sklearn import svm
from sklearn.neighbors import KNeighborsRegressor
from sklearn.neighbors import RadiusNeighborsRegressor
from sklearn.metrics import mean_squared_error
from sklearn.metrics import explained_variance_score
from sklearn.metrics import mean_absolute_error

class Shares:
    def __init__(self, data):
        self.data = data
        self.col_head = data.columns.values.tolist()
    
    def date_access(self):
        """
            datetime with "y-m-d"
        """
        if 'date' not in self.col_head:
            print "date is no exit" 
            return None
        else:
            date = []
            for no in range(len(self.data['date'])):
                date.append(dt.datetime.strptime(self.data['date'][no], "%Y-%m-%d"))
            date_list = []
            for d in date:
                temp = []
                temp.append(d.year)
                temp.append(d.month)
                temp.append(d.day)
                date_list.append(temp)
            del self.data['date']
            # 重新生成一个dataframe
            date_frame = pd.DataFrame(date_list, columns=['year', 'month', 'day'])
            # dataframe合并
            self.data = pd.concat([date_frame, self.data], axis=1)

    def time_access(self):
        """
            datetime with "y-m-d h:m:s"    
        """
        if 'date' not in self.colhead:
            print "date is no exit"
            return None
        else:
            date = []
            for no in range(len(self.data['date'])):
                date.append(dt.datetime.strptime(self.data['date'][no], "%Y-%m-%d %H:%M:%S"))
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
            del self.data['date']
            date_frame = pd.DataFrame(date_list, columns=['year', 'month', 'day', 'hour', 'minute', 'second'])
            self.data = pd.concat([date_frame, self.data], axis=1)
    
    def get_att(self, s_att):
        """
            return a attribute according input 
        """
        temp = self.data[s_att]
        return temp

    def get_ema(self, col='close', ema_model='12', ema_molecule=11, ema_denominator=13, init_ema=0):
        """
            compute EMA(12) & EMA(26)
            EMA(12) = EMA(12) * 11/13 + close * 2/13
            EMA(26) = EMA(26) * 25/27 + close * 2/27
        """
        ema_model = 'ema_'+ema_model
        ema = []
        length = len(self.data[col]) - 1
        ema.append(init_ema * ema_molecule/ema_denominator + self.data[col][length] * (ema_denominator-ema_molecule)/ema_denominator)
        for i in range(1, len(self.data[col])):
            ema.append(ema[i-1] * ema_molecule/ema_denominator + self.data[col][length-i] * (ema_denominator-ema_molecule)/ema_denominator)
        ema.reverse()

        ema = pd.DataFrame(ema, columns=[ema_model])
        return ema

    def get_diff(self, first_ema, second_ema):
        """
            compute DIFF bitween EMA(12) & EMA(16)
            DIFF = EMA(12) - EMA(26)
        """
        first_ema_col_head = first_ema.columns.values.tolist()
        second_ema_col_head = second_ema.columns.values.tolist()
        diff_col_head = 'diff_'+first_ema_col_head[0]+"_"+second_ema_col_head[0]

        diff = []
        for i in range(len()):
            diff.append(first_ema[i] - second_ema[i])
        diff = pd.DataFrame(diff, columns=[diff_col_head])
        return diff

    def get_dea(self, diff, dea_molecule=8, dea_denominator=10):
        """
            compute MACD(DEA)
            DEA(MACD) = DEA * 8/10 + DIF * 2/10
        """
        diff_col_head = diff.columns.values.tolist()
        dea_head = 'dea_'+diff_col_head[0]

        dea = []
        init_dea = 0
        length = len(diff[col_head[0]]) - 1
        dea.append(init_dea * dea_molecule/dea_denominator + diff[col_head[0]][length] * (dea_denominator-dea_molecule)/dea_denominator)
    
        for i in range(1,len(diff)):
            dea.append(dea[i-1] * dea_molecule/dea_denominator + diff[col_head[0]][length - i] * (dea_denominator-dea_molecule)/dea_denominator)
        dea.reverse()
        
        dea = pd.DataFrame(dea, columns=[dea_head])
        return dea
    
    def get_macd(self, diff, dea):
        """
            compute BAR
            macd = 2 * (DIFF - DEA)
        """
        diff_col_head = diff.columns.values.tolist()
        dea_col_head = dea.columns.values.tolist()
        macd_col_head = 'macd_'+diff_col_head[0]+"_"+dea_col_head[0]

        macd = []
        for i in range(len(diff)):
            macd.append(2 * (diff[diff_col_head[0]][i] - dea[dea_col_head[0]][i]))
        
        macd = pd.DataFrame(macd, columns=[macd_col_head])
        return macd

    def Get_Boll(self):
        """
            compute boll
            
        """
        pass

    def Get_RSI(self):
        """
            RSI(N) = A / (A+B) * 100
        """
        Posi_Price_Change = 0
        Nega_Price_Change = 0
        self.GSI = []
        length = len(self.data['price_change']) - 5
        for i in range(length):
            for j in range(5):
                if self.data['price_change'][i+j] > 0:
                    Posi_Price_Change += self.data['price_change'][i+j]
                else:
                    Nega_Price_Change += self.data['price_change'][i+j]
            self.GSI.append(Posi_Price_Change * (Posi_Price_Change + Nega_Price_Change * (-1)) * 100)
        for i in range(5):
            self.GSI.append(self.GSI[length])
        return self.GSI

    def Get_ColHead(self):
        return self.colHead
    
    def sort(self):
        """
            sort the data by the year/month/day
        """
        self.data = self.data.sort_values(by=['year', 'month', 'day'], ascending=True)

    def Get_Data(self):
        """
            return the data set with some computed_value
        """
        self.Date_Access()
        EMA = self.Get_EMA()
        DIFF = self.Get_DIFF()
        MACD = self.Get_MACD()
        BAR = self.Get_BAR()
        self.data = pd.concat([self.data, EMA, DIFF, MACD, BAR], axis=1)
        self.sort()
        return self.data

    def Target_Plot(self, target, start, end):
        total_df = []
        if 'ma5' in target:
            total_df.append(self.data['ma5'][start:end])
        if 'ma10' in target:
            total_df.append(self.data['ma10'][start:end])
        if 'ma20' in target:
            total_df.append(self.data['ma20'][start:end])
        if 'v_ma5' in target:
            total_df.append(self.data['v_ma5'][start:end])
        if 'v_ma10' in target:
            total_df.append(self.data['v_ma10'][start:end])
        if 'v_ma20' in target:
            total_df.append(self.data['v_ma20'][start:end])
        if 'EMA_12' in target:
            total_df.append(self.EMA['EMA_12'][start:end])
        if 'EMA_26' in target:
            total_df.append(self.EMA['EMA_26'][start:end])
        if 'DIFF' in target:
            total_df.append(self.DIFF['DIFF'][start:end])
        if 'MACD' in target:
            total_df.append(self.MACD['MACD'][start:end])
        if 'BAR' in target:
            total_df.append(self.BAR['BAR'][start:end])
            
        total_df = pd.concat(total_df, axis=1)
        if 'BAR' in target:
            total_df.plot(kind='bar')
        else:
            total_df.plot()
        plt.show()


class DataAccess:
    def __init__(self, data):
        self.data = data
        self.colHead = data.columns.values.tolist()

    def encode(self):
        """
            onehot encoding
        """
        enc = preprocessing.OneHotEncoder()
        enc.fit(self.data['year'])

    def Get_corr(self):
        """
            compute correlation
        """
        print self.data.corr()

    def Select_Label(self, label):
        """
            select label
            divide into data and label
        """
        if label not in self.colHead:
            label = label + "is no exit"
            print(label)
        else:
            label_list = self.data[label]
            del self.data[label]
            self.label_y = np.array(label_list)
            self.label_y = self.label_y.tolist()
            self.label_y.reverse()

    def Select_Data(self, Selected_Att):
        """
            According input to select columns
        """
        final = []
        for sa in Selected_Att:
            final.append(self.data[sa])
        self.data_x = pd.concat(final, axis=1)
        self.data_x = np.array(self.data_x)
        self.data_x = self.data_x.tolist()
        self.data_x.reverse()
        self.data_x = preprocessing.scale(self.data_x)
    
    def Divide(self, num):
        """
            divide data_set into train_set and test_set
        """
        train_x = []
        test_x = []
        train_y = []
        test_y = []
        
        temp = 0
        for i in range(len(self.data_x)):
            if temp < num:
                train_x.append(self.data_x[i])
                train_y.append(self.label_y[i])
                temp += 1
            else:
                test_x.append(self.data_x[i])
                test_y.append(self.label_y[i])
                temp = 0
                
        return train_x,train_y,test_x,test_y
        
    def Get_Data(self):
        return self.data

    def Get_Data_x(self):
        return self.data_x

    def Get_Label_y(self):
        return self.label_y


class Model:
    def __init__(self):
        pass
    
    def NN_Build(self, train_x, train_y):
        """
            structure nerual network
        """
        self.NN = MLPRegressor(hidden_layer_sizes=(50), activation='relu', solver='adam', 
                               alpha=0.0001, batch_size='auto', learning_rate='adaptive', learning_rate_init=0.001)
        self.NN.fit(train_x, train_y)

    def NN_Predict(self, test):
        """
            nerual network
            predict the result with the input data
        """
        pre_result = self.NN.predict(test)
        return pre_result
    
    def DT_Build(self, train_x, train_y):
        """
            decision tree model
        """
        self.DT = tree.DecisionTreeRegressor()
        self.DT.fit(train_x, train_y)
    
    def DT_Predict(self, test):
        """
            predict the result with the input data
        """
        pre_result = self.DT.predict(test)
        return pre_result
    
    def SVM_Build(self, train_x, train_y):
        """SVM_Build"""
        self.clf = svm.SVR()
        self.clf.fit(train_x, train_y)
        
    def SVM_Predict(self, test):
        """SVM_Predict"""
        pre_result = self.clf.predict(test)
        return pre_result
    
    def KNN_Build(self, train_x, train_y):
        """KNN_Build"""
        self.kneigh = KNeighborsRegressor(n_neighbors=2)
        self.kneigh.fit(train_x, train_y)
        
    def KNN_Predict(self, test):
        """KNN_Predict"""
        pre_result = self.kneigh.predict(test)
        return pre_result
    
    def RNN_Build(self, train_x, train_y):
        """RNN_Build"""
        self.rneigh = RadiusNeighborsRegressor(radius=1.0)
        self.rneigh.fit(train_x, train_y)
    
    def RNN_Predict(self, test):
        """RNN_Predict"""
        pre_result = self.kneigh.predict(test)
        return pre_result

    def pre_plot(self, train_y, pre_result, start, end, ti):
        """
            compare the predict_result with the true label
        """
        train_y = pd.DataFrame(train_y[start:end], columns=['train_y'])
        pre_result = pd.DataFrame(pre_result[start:end], columns=['pre_result'])
        result = pd.concat([train_y, pre_result], axis=1)
#         print result
        result.plot(title=ti)
        plt.show()
        
    def Get_MSE(self, test_y, pre_result):
        """
            compute MSE between label & pre_result
            MSE = sum(pow(test_y-pre_result,2)) / len(test_y)
        """
        diff = []
        for i in range(len(test_y)):
            diff.append(pow(test_y[i] - pre_result[i],2))
        result = np.sum(diff) / len(test_y)
        print 'MSE: ',result
    
    def Algorithm_compare(self, train_x, train_y, test_x, test_y):
        """plot the results precited by the models"""
        self.NN_Build(train_x, train_y)
        self.DT_Build(train_x, train_y)
        self.SVM_Build(train_x, train_y)
        self.KNN_Build(train_x, train_y)
        self.RNN_Build(train_x, train_y)
        self.NN_result = self.NN_Predict(test_x)
        self.DT_result = self.DT_Predict(test_x)
        self.SVM_result = self.SVM_Predict(test_x)
        self.KNN_result = self.KNN_Predict(test_x)
        self.RNN_result = self.RNN_Predict(test_x)
        
        self.pre_plot(test_y, self.NN_result, 0, len(self.NN_result), 'NN_result')
        self.Evaluate(test_y, self.NN_result)
        self.pre_plot(test_y, self.DT_result, 0, len(self.DT_result), 'DT_result')
        self.Evaluate(test_y, self.DT_result)
        self.pre_plot(test_y, self.SVM_result, 0, len(self.SVM_result), 'SVM_result')
        self.Evaluate(test_y, self.SVM_result)
        self.pre_plot(test_y, self.KNN_result, 0, len(self.KNN_result), 'KNN_result')
        self.Evaluate(test_y, self.KNN_result)
        self.pre_plot(test_y, self.RNN_result, 0, len(self.RNN_result), 'RNN_result')
        self.Evaluate(test_y, self.RNN_result)
        
    def Evaluate(self, test_y, pre_result):
        MSE = mean_squared_error(test_y, pre_result)
        MAE = mean_absolute_error(test_y, pre_result)
        EVS = explained_variance_score(test_y, pre_result)
        print 'MSE: ',MSE
        print 'MAE: ',MAE
        print 'EVS: ',EVS


if __name__ == "__main__":
    #读文件
    data = pd.read_csv("data/1day/000001.csv")
    
    #股票对象
    shares = Shares(data)
    data = shares.Get_Data()
    
    length = len(data['ma5'])
    shares.Target_Plot(['ma5','ma10','ma20'], length-20, length)
    shares.Target_Plot(['v_ma5','v_ma10','v_ma20'], length-20, length)
    shares.Target_Plot(['EMA_12','EMA_26'],length-20,length)
    shares.Target_Plot(['DIFF','MACD','BAR'],length-20,length)
    
    #数据处理对象
    DA = DataAccess(data)
#     DA.Get_corr()
    DA.Select_Label('price_change')
    cols = ['year', 'month', 'day', 'open', 'high', 'close', 'low', 'volume', 'p_change', 
            'ma5', 'ma10', 'ma20', 'v_ma5', 'v_ma10', 'v_ma20', 'EMA_12', 'EMA_26', 'turnover','DIFF','MACD','BAR']
    DA.Select_Data(cols)
    train_x,train_y,test_x,test_y=DA.Divide(4)

    #模型对象
    model = Model()
    model.Algorithm_compare(train_x, train_y, test_x, test_y)
