from Adafruit_IO import Client, RequestError, Feed
import serial                   #se importa libreria de comunicacion serial
import time
import tkinter as tk            #se importa libreria de GUI
from tkinter import *           #se importa libreria de GUI

ADAFRUIT_IO_KEY = "aio_KsXJ18VvooDg0DEzKYz76LwC1w0t"
ADAFRUIT_IO_USERNAME ="pabloha"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

contador1_feed = aio.feeds("lab5")
aio.send_data(contador1_feed.key, 0)
contador1_data = aio.receive(contador1_feed.key)
print(f'digital signal 1: {contador1_data.value}')

contador2_feed = aio.feeds("lab5-1")
contador2_data = aio.receive(contador2_feed.key)
print(f'digital signal 2: {contador2_data.value}')

contador3_feed = aio.feeds("lab5-2")
contador3_data = aio.receive(contador3_feed.key)

root = Tk()                     #se le da nombre al objeto principal

dato=serial.Serial()        #declarar puerto serial y braudeaje
dato.baudrate = 9600  # set Baud rate to 9600
dato.bytesize = 8     # Number of data bits = 8
dato.parity   ='N'    # No parity
dato.stopbits = 1     # Number of Stop bits = 1

dato.timeout=3
dato.port='COM1'    #Se define puerto
dato.open()    

titulo=tk.Label(root,text = "Laboratorio 5") #texto como titulo de GUI
titulo.pack()

dato.flushInput()
dato.flushOutput()

dato.read_until(b'\n', 4)
cuenta1 = dato.read_until(b'\n', 4)
print(cuenta1)
cuenta1 = int(cuenta1)
#print(type(cuenta1))
#cuenta1 = int.from_bytes(cuenta1, "big")
#print(type(cuenta1))
#print(cuenta1)
aio.send_data(contador1_feed.key, cuenta1)
root.counter = 0 

C1 = Label(root, text="Puerto B: " + str(cuenta1))
C1.pack()

C2 = Label(root, text="Puerto D: " + str(root.counter))                      
C2.pack()

C3 = Label(root, text ="Puerto A: " + contador3_data.value)
C3.pack()


aio.send_data(contador2_feed.key, root.counter)

def suma():                                          
    root.counter += 1
    if root.counter < 0:
        root.counter = 255
    if root.counter > 255:
        root.counter = 0
    C2['text'] = 'Puerto D: ' + str(root.counter)
    aio.send_data(contador2_feed.key, root.counter)
    dato.write(b'+')   #se manda + en ascii
    time.sleep(0.05)
    dato.write(b'\r')

#se define funcion para restar
def resta():                                          
    root.counter -= 1
    if root.counter < 0:
        root.counter = 255
    if root.counter > 255:
        root.counter = 0
    C2['text'] = 'Puerto D: ' + str(root.counter)
    aio.send_data(contador2_feed.key, root.counter)
    dato.write(b'-')   #se manda - en ascii
    time.sleep(0.05)
    dato.write(b'\r')

def enviar():       
    contador3_feed = aio.feeds("lab5-2")
    contador3_data = aio.receive(contador3_feed.key)
    C3['text'] = ('Puerto A: ' + contador3_data.value)           
    cuenta = int(contador3_data.value)
    C = cuenta/100
    C = int(C)
    D = (cuenta-C*100)/10
    D = int(D)
    U = (cuenta-C*100-D*10)
    U = int(U)
    dato.flushInput()
    dato.flushOutput()
    dato.write(str(C).encode('ascii'))
    time.sleep(0.1)
    dato.write(str(D).encode('ascii'))
    time.sleep(0.1)
    dato.write(str(U).encode('ascii'))
    time.sleep(0.1)
    dato.write(b'\n')
    time.sleep(0.1)
    dato.reset_input_buffer()
    dato.read_until(b'\n', 4)
    cuenta1 = dato.read_until(b'\n', 4)
    #cuenta1 = int.from_bytes(cuenta1, "big")
    cuenta1 = int(cuenta1)
    C1['text'] = ('Puerto B: ' + str(cuenta1))
    aio.send_data(contador1_feed.key, cuenta1)





#titulo de la ventana
root.title("Laboratorio 5")   #le pones titulo al objeto
root.minsize(400,200)                           #le decis el tamaño a la ventana

#boton de suma
b1 = Button(root, text="Suma", command=suma)
b1.place(x=150, y=90)

#boton de resta
b2 = Button(root, text="Resta", command=resta)
b2.place(x=200,y=90)

#boton enviar
b3 = Button(root, text="Enviar", command = enviar)
b3.place(x=175, y=125)





root.mainloop()     #Main loop



