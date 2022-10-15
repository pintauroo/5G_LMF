from locust import HttpUser, TaskSet, task, between, constant, events, LoadTestShape

import logging
import time
import greenlet
import time
import pandas as pd



address="http://192.168.70.132:9080"

class UserTasks(TaskSet):
    # one can specify tasks like this
    # tasks = [index, stats]

    def on_start(self):
        time.sleep(0.5)

    @task
    def localize_ue(self):
        start = time.time()
        self.client.get("/determine_location/"+str(greenlet.getcurrent().minimal_ident))
        logging.info("%s, %s",greenlet.getcurrent(), str(time.time() - start))



class User100(HttpUser):
    wait_time = constant(0.1)
    tasks = [UserTasks]
    host = address


class User200(HttpUser):
    wait_time = constant(0.2)
    tasks = [UserTasks]
    host = address


class User300(HttpUser):
    wait_time = constant(0.3)
    tasks = [UserTasks]
    host = address


class User400(HttpUser):
    wait_time = constant(0.4)
    tasks = [UserTasks]
    host = address


class User500(HttpUser):
    wait_time = constant(0.5)
    tasks = [UserTasks]
    host = address




class StagesShape(LoadTestShape):

    df = pd.read_csv("/home/giuseppe/andrea/locust/active-ues-begona-03062020_between730-1300.txt", sep=" ", header=None, names=['time', 'num'])
    count=0
    max = int(df.num.count())
    def tick(self):
        time.sleep(2)
        users = int((self.df.num[self.count]))
        tick_data = (users,users) 
        self.count+=1
        if self.count<self.max:
            return tick_data
        return None


# @events.quitting.add_listener
# def _(environment, **kw):
#     if environment.stats.total.fail_ratio > 0.01:
#         logging.error("Test failed due to failure ratio > 1%")
#         environment.process_exit_code = 1
#     elif environment.stats.total.avg_response_time > 200:
#         logging.error("Test failed due to average response time ratio > 200 ms")
#         environment.process_exit_code = 1
#     elif environment.stats.total.get_response_time_percentile(0.95) > 800:
#         logging.error("Test failed due to 95th percentile response time > 800 ms")
#         environment.process_exit_code = 1
#     else:
#         environment.process_exit_code = 0