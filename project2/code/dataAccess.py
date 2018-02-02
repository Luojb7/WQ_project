# coding: utf-8
import numpy as np
import pandas as pd
from sklearn import preprocessing
import pickle as pk
import indicatorGallexy as ig

class DataAccess:
    def __init__(self, data):
        self.data = data
        self.colHead = data.columns.values.tolist()

    def oneHotEncode(self,colname_list):
        """独热编码"""
        enc = preprocessing.OneHotEncoder()
        data_temp=[]
        for cl in colname_list:
            data_temp.append(self.data[cl])
            del self.data[cl]
        data_temp = pd.concat(data_temp,axis=1)
        enc.fit(data_temp)
        data_temp = enc.transform(data_temp).toarray()
        data_temp = pd.DataFrame(data_temp)
        self.data = pd.concat([data_temp, self.data], axis=1)
        self.colHead = self.data.columns.values.tolist()

    def getCorr(self):
        print self.data.corr()

    def selectLabel(self, label):
        """选择预测对象, 把该列值单独取出"""
        if label not in self.colHead:
            label = label + "is no exit"
            print(label)
        else:
            label_list = self.data[label]
            del self.data[label]
            self.label_y = np.array(label_list)
            self.label_y = self.label_y.tolist()
            # self.label_y.reverse()

    def selectAtt(self, selected_att):
        """选择训练的属性集"""
        final = []
        for sa in selected_att:
            final.append(self.data[sa])
        self.data_x = pd.concat(final, axis=1)
        self.data_x = np.array(self.data_x)
        self.data_x = self.data_x.tolist()
        # self.data_x.reverse()
        self.data_x = preprocessing.scale(self.data_x)

    def divideData(self, num):
        """划分训练集和验证集, 以num+1为一组进行划分，并进行移位操作"""
        train_x = []
        test_x = []
        train_y = []
        test_y = []
        
        temp = 0
        for i in range(len(self.data_x)-1):
            if temp < num:
                train_x.append(self.data_x[i])
                train_y.append(self.label_y[i+1])
                temp += 1
            else:
                test_x.append(self.data_x[i])
                test_y.append(self.label_y[i+1])
                temp = 0
                
        return train_x,train_y,test_x,test_y

    def storeToPickle(self, data, file_path):
        """序列化操作"""
        pickle_file = open(file_path, 'wb')
        pk.dump(data, pickle_file)
        
    def getData(self):
        return self.data

    def getColHead(self):
        return self.colHead

    def getDataX(self):
        return self.data_x

    def getLabelY(self):
        return self.label_y

if __name__ == "__main__":
    data = pd.read_csv("data/1day/000001.csv")
    xmIG = ig.IndicatorGallexy(data)

    xmIG.dateAccess('date')
    data = xmIG.getData()

    xmDA = DataAccess(data)
    # xmDA.oneHotEncode(['year', 'month', 'day'])
    xmDA.selectLabel('year')
