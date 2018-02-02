## 代码说明
### IndicatorGallexy
* isValidDate(self, str) & isValidTime(slef, str)
	判断传入字符串是否是可被处理的日期或时间格式


* dateAccess(self, date_col_head)
	* 根据前面的格式判断结果进行日期处理
	* date_col_head为传入字符串，代表dataframe中日期一列的列名
	* 5min数据和1day数据可以共同使用本函数进行处理


* getEMA(self, col='close', ema_model='ema12', ema_molecule=11, ema_denominator=13, init_ema=0)
	* 计算EMA指标
	* col: 被选择计算的列名
	* ema_model: 计算的ema指标类型
	* ema_molecule: 计算公式中的分子
	* ema_denominator: 计算公式中的分母
	* init_ema: 计算初始值


* getDIFF(self, first_ema, second_ema, col='close')
	* 计算DIFF指标
	* first_ema: 第一个ema指标
	* second_ema: 第二个ema指标
	* col: 被选择计算的列名
	

* getDEA(self, diff, dea_molecule=8, dea_denominator=10, init_dea=0, col='close')
	* 计算DEA指标


* getMACD(self, diff, dea, col='close')
	* 计算MACD指标


* getRSI(self, selected_att='price_change', power=100)
	* 计算RSI指标


* plotData(self, col_list):
	* 根据输入的属性列表，画出对比图像


### DataAccess
* selectLabel(self, label)
	* label: 字符串类型，代表选择的要预测的Y


* selectAtt(self, selected_att)
	* selected_att: 字符串list，代表要选择的用于训练的属性集


* divideData(self, num)
	* num：表示以num+1个样本为一组划分训练集和验证集，并执行y移位操作

* storeToPickle(self, data, file_path):
	* 把数据或者模型转存成pickle文件
	


### Model
* modelBuild(self, train_x, train_y, selected_model='NN')
	* 根据selected_model来选择构建的机器学习模型


* modelPredict(self, test_x)
	* 预测函数


* plotResult(self, test_y, pre_result, ti)
	* 使用画图方法比较预测结果与真实值的差别
	* ti: 字符串类型，用户plot()函数的title属性


* algorithmCompare(self, train_x, train_y, test_x, test_y, model_list=[])
	* 模型比较函数，根据输入的model_list，分别画出预测结果图和计算出相应模型评价指标
	* model_list: 字符串list


* algorithmEvaluate(self, test_y, pre_result)
	* 模型评价指标计算函数，包括MSE, MAE, EVS

### FastResearchData
* getCode(self, file_path):
	* 获取路径中的股票编号

* loadFromDataFrame(self, stock_code, df):
	* 把dataframe对象存入字典

* loadStockData(self, stock_code, file_type='csv', sheet_name=''):
	* 根据输入的文件类型、股票编号，生成对应的文件路径，并通过该文件路径读取数据
	* 用于数据更新

* loadFromFile(self, file_path, file_type='csv', sheet_name=''):
	* 根据数据的文件类型以及文件路径读取数据，存入字典

