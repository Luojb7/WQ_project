# coding: utf-8
import pandas as pd
import os
from sklearn import preprocessing
import pickle as pk


class FastResearchData:
    def __init__(self, dir_path):
        self.stock_dict = {}
        self.dir_path = dir_path

    def getCode(self, file_path):
        """截取文件路径中的股票编号"""
        code_start_position = file_path.rfind('/')
        code_end_position = file_path.rfind('.')
        code = file_path[code_start_position+1:code_end_position]
        return code

    def loadFromDataFrame(self, stock_code, df):
        """把dataframe对象存入字典"""
        if stock_code not in self.stock_dict.keys():
            self.stock_dict[stock_code] = df
        else:
            pd.concat([self.stock_dict[stock_code], df], axis=0)

    def loadStockData(self, stock_code, file_type='csv', sheet_name=''):
        """根据股票编号生成文件路径，并读取文件数据，存入字典. 更新数据用"""
        if file_type == 'csv':
            df = pd.read_csv(os.path.join(self.dir_path, stock_code+'.csv'))
        elif file_type == 'pickle':
            df = pd.DataFrame(pk.load(open(os.path.join(self.dir_path, stock_code+'.pkl'), 'rb')))
        elif file_type == 'hdf':
            df = pd.read_hdf(os.path.join(self.dir_path, stock_code+'.hdf'), 'df')
        elif file_type == 'excel':
            df = pd.DataFrame(pd.read_excel(os.path.join(self.dir_path, stock_code+'.xlsx'), sheet_name))
        else:
            print "this type of file can not be read"
            return 
        if stock_code not in self.stock_dict.keys():
            self.stock_dict[stock_code] = df
        else:
            pd.concat([self.stock_dict[stock_code], df], axis=0)

    def loadFromFile(self, file_path, file_type='csv', sheet_name=''):
        """根据输入的文件类型，读取文件数据，存入字典"""
        stock_code = self.getCode(file_path)
        if file_type == 'csv':
            df = pd.read_csv(file_path)
        elif file_type == 'pickle':
            df = pd.DataFrame(pk.load(open(file_path,'rb')))
        elif file_type == 'hdf':
            df = pd.read_hdf(file_path, 'df')
        elif file_type == 'excel':
            df = pd.DataFrame(pd.read_excel(file_path, sheet_name))
        else:
            print "this type of file can not be read"
            return
        self.loadFromDataFrame(stock_code, df)

    # def loadFromCSV(self, file_path):
    #     """从csv文件中读取数据"""
    #     code = self.getCode(file_path)
    #     df = pd.read_csv(file_path)
    #     self.loadFromDataFrame(code, df)

    # def loadFromPickle(self, stock_code, file_name):
    #     """从Pickle中读取数据，并存入字典"""
    #     pickle_file = open(file_name, 'rb')
    #     df = pd.DataFrame(pk.load(pickle_file))
    #     self.loadFromDataFrame(stock_code, df)

    # def loadFromHDF5(self, stock_code, file_path):
    #     """从hdf5文件中读取数据，并存入字典"""
    #     hdf = pd.DataFrame(pd.read_hdf(file_path, 'df'))
    #     self.loadFromDataFrame(stock_code, hdf)

    # def loadFromExcel(self, stock_code, file_path, sheet_name):
    #     """从excel文件中读取数据，并存入字典"""
    #     excel = pd.DataFrame(pd.read_excel(file_path, sheet_name))
    #     self.loadFromDataFrame(stock_code, excel)

    def getDataFrame(self, stock_code):
        """返回某一股票数据"""
        if stock_code not in self.stock_dict.keys():
            print "the code is not in the data set"
            return None
        return self.stock_dict[stock_code]
    
    def setDataFrame(self, stock_code, df):
        """更新某一股票数据"""
        if stock_code not in self.stock_dict.keys():
            print "the code is not in the data set"
            return None
        self.stock_dict[stock_code] = df

    def getStockDict(self):
        """返回整个字典"""
        return self.stock_dict

if __name__ == "__main__":
    frd = FastResearchData("data/1day/")
    frd.loadFromFile("data/1day/000001.csv", 'csv')
    stock_dict = frd.getStockDict()
    print stock_dict.keys()
