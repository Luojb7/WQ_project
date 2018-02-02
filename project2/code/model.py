# coding: utf-8
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn import preprocessing
from sklearn.neural_network import MLPRegressor
from sklearn import tree
from sklearn import svm
from sklearn.neighbors import KNeighborsRegressor
from sklearn.neighbors import RadiusNeighborsRegressor
from sklearn.metrics import mean_squared_error
from sklearn.metrics import explained_variance_score
from sklearn.metrics import mean_absolute_error


class Model:
    def __init__(self):
        self.my_model = None

    def modelBuild(self, train_x, train_y, selected_model='NN'):
        """训练模型"""
        if selected_model == 'NN':
            self.my_model = MLPRegressor(hidden_layer_sizes=(50), activation='relu', solver='adam', alpha=0.0001, batch_size='auto', learning_rate='adaptive', learning_rate_init=0.001)
        elif selected_model == 'DT':
            self.my_model = tree.DecisionTreeRegressor()
        elif selected_model == 'SVM':
            self.my_model = svm.SVR()
        elif selected_model == 'KNN':
            self.my_model == KNeighborsRegressor(n_neighbors=2)
        elif selected_model == 'RNN':
            self.my_model == RadiusNeighborsRegressor(radius=1.0)
        else:
            print "this model can not be built"
            return

        self.my_model.fit(train_x, train_y)

    def modelPredict(self, test_x):
        """模型预测，返回预测结果"""
        pre_result = self.my_model.predict(test_x)
        return pre_result

    def plotResult(self, test_y, pre_result, ti):
        """结果展示"""
        test_y = pd.DataFrame(test_y, columns=['train_y'])
        pre_result = pd.DataFrame(pre_result, columns=['pre_result'])

        result = pd.concat([test_y, pre_result], axis=1)
#         print result
        result.plot(title=ti)
        plt.show()
    
    def algorithmCompare(self, train_x, train_y, test_x, test_y, model_list=[]):
        """模型比较"""
        evaluate = []
        evaluate_colname = ['MSE','MAE','EVS']
        for ml in model_list:
            self.modelBuild(train_x, train_y, ml)
            pre_result = self.modelPredict(test_x)
            print ml+": "
            self.plotResult(test_y, pre_result, ml)
            mse_temp, mae_temp, evs_temp = self.algorithmEvaluate(test_y, pre_result)
            evaluate.append([mse_temp,mae_temp,evs_temp])
        evaluate = pd.DataFrame(evaluate, columns=evaluate_colname)
        evaluate.plot(kind='bar')
        plt.show()
        
    def algorithmEvaluate(self, test_y, pre_result):
        """计算评价指标"""
        mse = mean_squared_error(test_y, pre_result)
        mae = mean_absolute_error(test_y, pre_result)
        evs = explained_variance_score(test_y, pre_result)
        print 'MSE: ',mse
        print 'MAE: ',mae
        print 'EVS: ',evs
        return mse, mae, evs


if __name__ == "__main__":
    pass