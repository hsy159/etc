import xlrd
import pandas as pd
import matplotlib.pyplot as plt
import xlwt
import numpy as np
import sklearn
from sklearn.linear_model import Lasso
from sklearn.linear_model import LassoCV
from sklearn import cross_validation
from sklearn import linear_model
'''
from pybrain.datasets import ClassificationDataSet
from pybrain.utilities import percentError
from pybrain.tools.shortcuts import buildNetwork
from pybrain.supervised.trainers import BackpropTrainer
from pybrain.structure.modules import SoftmaxLayer
from pybrain.tools.xml.networkwriter import NetworkWriter
from pybrain.datasets import SupervisedDataSet
from pybrain.tools.xml.networkreader import NetworkReader
'''



def read_xls(file_name):
	xl = pd.ExcelFile(file_name)
	xl.sheet_names
	df1 = xl.parse("overpass")
	df2 = xl.parse("not_overpass")
	#df1.head()
	#df2.head()
	return df1, df2

def to_list(d, df, start):
	for i in range(1, len(df.columns)+1):
		for j in range(0,len(df[i])):
			if type(df[i][j]) == unicode:
				a = df[i][j].split(' ')
				a_len = len(a)
				if a_len == 3:
					a_per = a[a_len-1].split('%')[0]
					if a[0] in d:
						d[a[0]][start + i] = int(a_per)
					else:
						d[a[0]] = {start + i:int(a_per)}
				elif a_len > 3:
					a_name = ''
					a_per = a[a_len-1].split('%')[0]
					for k in range(0,len(a)-1):
						#if (k > 0) & (k < len(a)-2):
							#a_name += ' '
						a_name += a[k]
					if a_name in d:
						d[a_name][start + i] = int(a_per)
					else:
						d[a_name] = {start + i:int(a_per)}
	return d

def compare_same_key(d1,d2):
	key_lst = []
	for key in d1:
		for key2 in d2:
			if key == key2:
				print key
				print d1[key]
				print d2[key]
				key_lst.append(key)
	return key_lst

def fillin_zero(d,key):
	for i in range(1,201): 
		if i not in d[key]:
			d[key][i] = int(0)

def drawing(key):
	plt.plot(d1[key])
	plt.plot(d2[key])
	plt.show()

def dict_to_excelfile(lst,df_name,name):
	df_name = pd.DataFrame.from_dict(lst)
	pd.DataFrame.to_excel(df_name,name)


def split_dataset_cv(x, y, shuffle=False):  
    if (shuffle==False):
        len_t = int(0.6*len(x))
        len_c = int(0.8*len(x))

        x_train = x[:len_t]
        x_cv = x[len_t:len_c]
        x_test = x[len_c:]
        y_train = y[:len_t]
        y_cv = y[len_t:len_c]
        y_test = y[len_c:]
    else:
        a = range(len(x))
        np.random.shuffle(a)
        len_t = int(0.6*len(x))
        len_c = int(0.8*len(x))
        
        len_t_s = a[:len_t]
        len_c_s = a[len_t:len_c]
        len_test_s = a[len_c:]
        
        x_train = x[len_t_s]
        x_cv = x[len_c_s]
        x_test = x[len_test_s]
        y_train = y[len_t_s]
        y_cv = y[len_c_s]
        y_test = y[len_test_s]
        
    return (x_train, x_cv, x_test, y_train, y_cv, y_test)

def split_dataset_no_cv(x, y, shuffle=False):  
    if (shuffle==False):
        len_t = int(0.7*len(x))

        x_train = x[:len_t]
        x_test = x[len_t:]
        y_train = y[:len_t]
        y_test = y[len_t:]
    else:
        a = range(len(x))
        np.random.shuffle(a)
        len_t = int(0.7*len(x))
        
        len_t_s = a[:len_t]
        len_test_s = a[len_t:]
        
        x_train = x[len_t_s]
        x_test = x[len_test_s]
        y_train = y[len_t_s]
        y_test = y[len_test_s]
        
    return (x_train, x_test, y_train, y_test)



df1, df2 = read_xls('100pic.xls')
d1 = dict()
d1 = to_list(d1, df1, 0)
d1 = to_list(d1, df2, 100)

for key in d1:
	fillin_zero(d1,key)

all_df = pd.DataFrame.from_dict(d1)
all_df['Result'] = 1
all_df['Result'][101:202] = 0

pd.DataFrame.to_excel(all_df, 'final_data.xls')

data_matrix = pd.DataFrame.as_matrix(all_df)
matrix_len = len(data_matrix[0])

x_data = data_matrix[:,:matrix_len-1]
y_data = data_matrix[:,matrix_len-1]

#(x_train, x_cv, x_test, y_train, y_cv, y_test) = split_dataset_cv(x_data, y_data, True)
(x_train, x_test, y_train, y_test) = split_dataset_no_cv(x_data, y_data, True)

print "Train set: " , len(x_train)
#print "CV set    : ", len(x_cv)
print "Test set : ", len(x_test)

'''svc_model = SVC()
svc_model.fit(x_train, y_train)
pred_cv = svc_model.predict(x_cv)
prf = sklearn.metrics.precision_recall_fscore_support(y_cv, pred_cv)
print prf[2]

pred_test = svc_model.predict(x_test)
prf_test = sklearn.metrics.precision_recall_fscore_support(y_test, pred_test)
print prf_test[2]'''

'''
neural_network = NeuralNetwork()

print "Random starting synaptic weights: "
print neural_network.synaptic_weights

# The training set. We have 4 examples, each consisting of 3 input values
# and 1 output value.
training_set_inputs = x_train
training_set_outputs = y_train.T

# Train the neural network using a training set.
# Do it 10,000 times and make small adjustments each time.
neural_network.train(training_set_inputs, training_set_outputs, 10000)

print "New synaptic weights after training: "
print neural_network.synaptic_weights

# Test the neural network with a new situation.
#print "Considering new situation [1, 0, 0] -> ?: "
print neural_network.think(x_cv[0])
'''


pred_result = []
clf = Lasso(alpha = 0.1)
clf.fit(x_train,y_train)
pred = clf.predict(x_test)
pred = pred.tolist()
for i in pred:
	if i < 0.5:
		pred_result.append(0)
	else:
		pred_result.append(1)

print np.array(pred_result)
print y_test

# Accuracy checking
same_cnt = 0
for i in range(1,len(y_test)):
	if y_test[i] == pred_result[i]:
		same_cnt += 1

print "Accuracy : ", (float(same_cnt)/len(y_test))




'''
lr = linear_model.LinearRegression()
predicted = cross_validation.cross_val_predict(lr, x_data, y_data, cv=5)

fig, ax = plt.subplots()
ax.scatter(y_data, predicted)
ax.plot([0,1], [0,1], 'k--', lw=4)
ax.set_xlabel('Measured')
ax.set_ylabel('Predicted')
plt.show()
'''




