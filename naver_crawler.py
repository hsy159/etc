from bs4 import BeautifulSoup
import requests
from pattern.web import Element
from pattern import web
import xlrd
import xlsxwriter



def get_headline_page(date_rank):   # naver 경제부문의 news html parse
    print "fetching " , date_rank

    URL = "http://news.naver.com/main/ranking/popularDay.nhn?rankingType=popular_day&sectionId=101&date="+date_rank
    head_r = requests.get(URL)
    soup = BeautifulSoup(head_r.text)
    return soup


def parsing_headline(date):     #1~30위까지 rank 30위까지 crawl
    html=get_headline_page(date)
    element = web.Element(html)

    lst =[]

    for i in range(3):
        a = html.findAll('li',{'class':'num'+str(i+1)})
        lst.append(str(a[0]).split('<dt>')[1].split('title=')[1].split('>')[0].decode('utf-8'))
    for i in range(7):
        b = html.findAll('li',{'class':'gnum'+str(i+4)})
        #print 'Rank' + str(i+4) +': ' + str(b[0]).split('title=')[1].split('"')[1]
        lst.append((str(b[0]).split('title=')[1].split('"')[1]).decode('utf-8'))
    for i in range(10):
        c = html.findAll('li',{'class':'gnum'+str(i+11)})
        #print 'Rank' + str(i+11) +': ' + str(c[0]).split('title=')[1].split('"')[1]
        lst.append((str(c[0]).split('title=')[1].split('"')[1]).decode('utf-8'))
    for i in range(10):
        d = html.findAll('li',{'class':'gnum'+str(i+21)})
        #print 'Rank' + str(i+21) +': ' + str(d[0]).split('title=')[1].split('"')[1]
        lst.append((str(d[0]).split('title=')[1].split('"')[1]).decode('utf-8'))
    return lst



def date_creater(date_num):  # 날짜 수 입력하면 오늘부터 전날까지 date list 만듦
    arr=[]
    for i in range(date_num):
        a = str(datetime.datetime.now()-datetime.timedelta(i))[0:4]
        b = str(datetime.datetime.now()-datetime.timedelta(i))[5:7]
        c = str(datetime.datetime.now()-datetime.timedelta(i))[8:10]
        arr.append(a+b+c)
    return arr

test = date_creater(100)
lst_tmp =[]

for i in test:
    lst_tmp.append(parsing_headline(i))
    
workbook = xlsxwriter.Workbook('headline.xlsx')
worksheet = workbook.add_worksheet()

row = 1
col = 0

for headline in test:
    worksheet.write(row, col, headline)
    row+=1

row = 1
col = 1

for j in range(100):
    for i in lst_tmp[j]:
        worksheet. write(row, col, i)
        col+=1
col = 1
row+=1
workbook.close()
