from threading import Thread

def test_method(x):
  window.x_value = x

def test_thread():
  Titanium.API.runOnMainThread(test_method, 'threads rock!')

t = Thread(target=test_thread)
t.start()
t.join()
