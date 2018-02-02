# coding: utf-8
import argparse
import copy as cp
import pandas as pd
import indicatorGallexy as ig
import dataAccess as da
import model as md
import fastResearchData as frd


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-sc", "--stock_code", type=str, default='000001', help="the stock codes you want to load")
    parser.add_argument("-l", "--label", type=str, default="price_change", help="the label you want to predict")
    parser.add_argument("-d", "--date", type=str, default="date", help="the col name of date_data")
    parser.add_argument("-da","--del_att",type=str,default='year,month,day',help="the attributes you want to delete")
    parser.add_argument("-m", "--method", type=str, default='NN', help="select model in NN\DT\SVM\KNN\RNN")
    parser.add_argument("-r", "--ratio", type=int, default=9, help="the ratio used to divide train and test")
    parser.add_argument("-t", "--test", type=int, default=0, help="if you want to compare the results between all models, input 1")
    args = parser.parse_args()

    stock_code = args.stock_code
    print stock_code
    label = args.label
    del_att = args.del_att.split(',')
    date = args.date
    ratio = args.ratio
    test = args.test
    method = args.method

    dir_path = 'data/1day/'
    xmFRD = frd.FastResearchData(dir_path)
    xmFRD.loadStockData(stock_code)
    xmIG = ig.IndicatorGallexy(xmFRD.getDataFrame(stock_code))
    xmIG.dateAccess(date)
    xmIG.getData().to_csv("process_data/"+stock_code+'.csv', index=False)

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

    xmIG_data = xmIG.getData()
    # xmIG_data_col_head = xmIG.getColHead()

    xmDA = da.DataAccess(xmIG_data)
    # xmDA.oneHotEncode(['year', 'month', 'day'])
    select_att = xmDA.getColHead()
    xmDA.selectLabel(label)
    select_att.remove(label)
    for temp in del_att:
        select_att.remove(temp)
    print select_att
    xmDA.selectAtt(select_att)
    train_x, train_y, test_x, test_y = xmDA.divideData(ratio)

    xmModel = md.Model()

    if test == 1:
        model_list = ['NN', 'DT', 'SVM', 'KNN', 'RNN']
        xmModel.algorithmCompare(train_x, train_y, test_x, test_y, model_list)
    else:
        xmModel.modelBuild(train_x, train_y, method)
        pre_result = xmModel.modelPredict(test_x)
        xmModel.plotResult(test_y, pre_result, method)
        xmModel.algorithmEvaluate(test_y, pre_result)