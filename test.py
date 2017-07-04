#classify
import xlrd
import pandas as pd
import matplotlib.pyplot as plt
import xlwt

def read_xls(file_name):
	xl = pd.ExcelFile(file_name)
	xl.sheet_names
	df1 = xl.parse("overpass")
	df2 = xl.parse("not_overpass")
	#df1.head()
	#df2.head()
	return df1, df2

def to_list(df):
	d = dict()
	for i in range(1, len(df.columns)+1):
		for j in range(0,len(df[i])):
			if type(df[i][j]) == unicode:
				a = df[i][j].split(' ')
				a_len = len(a)
				if a_len == 3:
					a_per = a[a_len-1].split('%')[0]
					if a[0] in d:
						d[a[0]][i] = int(a_per)
					else:
						d[a[0]] = {i:int(a_per)}
				elif a_len > 3:
					a_name = ''
					a_per = a[a_len-1].split('%')[0]
					for k in range(0,len(a)-1):
						#if (k > 0) & (k < len(a)-2):
							#a_name += ' '
						a_name += a[k]
					if a_name in d:
						d[a_name][i] = int(a_per)
					else:
						d[a_name] = {i:int(a_per)}
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
	for i in range(1,101): #숫자는 지금 사용한 데이터, 필요하면 나중에 변경
		if i not in d[key]:
			d[key][i] = int(0)

def drawing(key):
	plt.plot(d1[key])
	plt.plot(d2[key])
	plt.show()

def dict_to_excelfile(lst,df_name,name):
	df_name = pd.DataFrame.from_dict(lst)
	pd.DataFrame.to_excel(df_name,name)


df1, df2 = read_xls('100pic.xls')
d1 = to_list(df1)
d2 = to_list(df2)

for key in d1:
	fillin_zero(d1,key)

for key in d2:
	fillin_zero(d2,key)

dict_to_excelfile(d1, 'overpass_df', 'overpass.xls')
dict_to_excelfile(d2, 'nonoverpass_df', 'nonoverpass.xls')


