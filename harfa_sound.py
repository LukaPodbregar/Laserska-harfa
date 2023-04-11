import serial # Vključi knjižnico za serijsko komunikacijo
import serial.tools.list_ports
import time # Vključi knjižnico za čas
import pygame.midi # Vključi knjižnico za midi zvoke iz knjižnice pygame

comport = "" # Prazen niz za določitev pravilnih komunikacijskih vrat (ang. COM port)
while comport=="":
        ports = list(serial.tools.list_ports.comports()) # Shrani seznam vseh naprav na komunikacijskih vratih računalnika
        for p in ports:
                if "CH340" in p.description: # Preveri, če je na vratih zaznan čip CH340
                        comport = p.name # Izberi vrata s čipom CH340
        time.sleep(1) # Poskusi ponovno čez 1 s
print("\nLaser harp connected!") # Izpiši, da je laserska harfa povezana      
serialPort = serial.Serial( # Nastavi parametre komunikacije (vrata, hitrost prenosa, velikost bajta, čas izteka, število stop bitov)
    port=comport, baudrate=9600, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

serialString = ""  # Prazen niz za shranjevanje prejetih podatkov
pygame.midi.init() # Inicializacija knjižnice midi
player = pygame.midi.Output(0) # Za predvajalnik zvoka izbran računalnik
player.set_instrument(46) # Nastavi inštrument
note = 48 # Definicija spremenljivke za shranjevanje tonov
note_old = 10 # Definicija spremenljivke za shranjevanje pravkar predvajanega tona
start = 0 # Definicija spremenljivke za shranjevanje časa
end = 0 # Definicija spremenljivke za shranjevanje časa

while 1: # Glavna while zanka
    if serialPort.in_waiting > 0: # Čakaj dokler mikrokrmilnik ne pošilja nič
        serialString = serialPort.read() # Preberi sprejete podatke
        try: 
                if (serialString): # Preveri če sprejeti podatki veljavni
                        recieved_string = serialString.decode("Ascii") # Prevedi prejet bajt v ascii znak
                        end = time.time() # Zapiši trenutni čas
                        if(recieved_string != note_old): # Preveri, če je prejet podatak različen kot prejšnji (končaj prevajanje tona)
                                player.note_off(note, 127) # Ugasni predvajan ton
                                if recieved_string == "0": # Preveri kateri ton je in shrani pripadajoč ton
                                        note = 48
                                elif recieved_string == "1":
                                        note = 50
                                elif recieved_string == "2":
                                        note = 52
                                elif recieved_string == "3":
                                        note = 53
                                elif recieved_string == "4":
                                        note = 55
                                elif recieved_string == "5":
                                        note = 57
                                elif recieved_string == "6":
                                        note = 59
                                note_old = recieved_string
                                player.note_on(note, 127) # Predvajaj ton
                                time.sleep(0.25) # Minimalen čas predvajanja tona
                                serialPort.reset_input_buffer() # Pobriši čakalno vrsto serijske komunikacije
                                start = time.time() # Zapiši trenutni čas 
                        elif (recieved_string == note_old and (end-start)<0.75): # Če je prejet podatek enak kot prejšnji in je čas prekinitve krajši kot 0,6 s, podaljšaj ton
                               time.sleep(0.25) # Če je enaka struna še kar prekinjena podaljšaj čas predvajanja
                               serialPort.reset_input_buffer() # Pobriši čakalno vrsto serijske komunikacije
                        else: # Če je čas prekinitve daljši od 0,6 s -> omogoči ponovno predvajanje tona
                              note_old = 10 # Nastavi spremenljivko na ton, ki ni mogoč
                              player.note_off(note, 127) # Ugasni predvajan ton
                              serialPort.reset_input_buffer() # Pobriši čakalno vrsto serijske komunikacije
        except:
            pass