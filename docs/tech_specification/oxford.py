# *-* coding:utf-8 *-*

import serial
import io
import time
import datetime as dt
import numpy as np
import threading
import Queue

class OxfordIsobusDevice(object):
    '''class which provides the controlling of Oxford isobus devices'''
    
    def __init__(self, port, adress = None):
        self.__port = port #takes COM port object
        self.__port.flush() #clears COM port buffer
        
        self.__wait = 0.01 #minimum waiting time between sending commands
        if adress == None:
            self.__adress = '' #control all devices simultanious
        else:
            self.__adress = '@%i' % (adress) #control only adressed device
            
        self.__port.write(unicode('%sQ2\r') % (self.__adress)) #set communication to '\r\n' mode for faster communication (port.readline wants \n as EOL indicator)
        self.__port.flush() #clears COM port buffer
        
    def write(self, command, answer = False):
        time.sleep(self.__wait) #minimum waiting time between sending commands
        if answer:
            self.__port.write(unicode('%s%s\r') % (self.__adress, command)) #no $ means RESPONSE if there is some
        else:
            self.__port.write(unicode('$%s%s\r') % (self.__adress, command)) #$ at the beginning means NO RESPONSE from device
        self.__port.flushOutput() #clearing the Output buffer
            
    def read(self):
        ret = self.__port.readline().strip('\r\n') #get the RESPONSE and remove the EOL indicators '\r\n'
        self.__port.flushInput() #clear the Input buffer
        if len(ret) == 0: #if there is no response return an empty string
            return ''
        else:
            if ret[0] == '?': #checks the respose for Error
                raise ValueError('The Oxford Instrument reported a ERROR "%s"!' % (ret))
            else:
                return ret #normal response
        
    def ask(self, command): #writes a command and asks for the response as soon a possible
        self.write(command, answer = True)
        return self.read()
    
    def changeAdress(self, adress=None):
        if adress == None:
            self.__adress = '' #control all devices simultanious
        else:
            self.__adress = '@%i' % (adress) #control only adressed device
    

class ILM200(object):
    '''Wrapper class for ILM200'''
    
    def __init__(self, port):
        self.__device = OxfordIsobusDevice(port, 5) #port should be COM port, 5 is standart device adress of ILM200
        
        # Dictionaries to convert Strings into Control parameters
        self.__dictControlState = {'LOCAL': 0, 'REMOTE+LOCKED': 1, 'REMOTE+UNLOCKED': 3}
        self.__dictParameter = {'CHA1LVL': 1, 'CHA2LVL': 2, 'CHA3LVL': 3, 'CHA1CUR': 6, 'CHA2CUR': 7, 'VALVEPOS': 10, 'CHA1IF': 11, 'CHA2IF': 12, 'CHA3IF': 13}
        self.__dictSysLockState = {'LOCKED': 0, '!UNLOCKED': 1, 'SLEEP': 1234, 'WAKEUP': 4321, 'UNLOCKY': 9999}
        self.__dictChannelUsage = {0: 'NOT USED', 1: 'NITROGEN LEVEL', 2: 'HELIUM LEVEL (pulsed mode)', 3: 'HELIUM LEVEL (contiuous mode)', 9: 'ERROR'}
        
        
    # ### Monitor Commands (always recognised) ### #
    
    def __setControlState(self, f):
        '''sets the control state of the device'''
        ret = self.__device.ask('C%i' % self.__dictControlState[f])
        print 'Set control state to "%s".' % f
        return ret
        
    def __setCommunicationProtocol(self, f):
        '''defines how the device answers (only 0 and 2 are available)'''
        self.__device.write('Q%i' % f)
        print 'Set communication portocoll to "%s".' % f
        
    def __getParameter(self, f):
        '''asks for Parameter to read out'''
        ret = self.__device.ask('R%i' % self.__dictParameter[f])
        return int(ret.lstrip('R'))
    
    def __setSystemLockState(self, f):
        '''sets the system locke state which gives acces to system commands'''
        ret = self.__device.ask('U%i' % self.__dictSysLockState[f])
        print 'Set system lock state to "%s".' % f
        return ret
        
    def __getVersion(self):
        '''asks for the Device and Software Version'''
        ret = self.__device.ask('V')
        return ret
    
    def __setOutputWaitingTime(self, time):
        '''sets the waiting time in miliseconds between character sending of device'''
        ret = self.__device.ask('W%i' % time)
        print 'Set output waiting time to %i ms.' % time
        return ret
        
    def __getDeviceState(self):
        '''reads out the state of the device'''
        ret = self.__device.ask('X')
        return ret
    
    
    # ### Control Commands (recognised only in REMOTE mode) ### #
    
    def __setDisplaySrc(self, f):
        '''defines the Parameter which is shown on Helium Display (DISP1)'''
        ret = self.__device.ask('F%i' % self.__dictParameter[f])
        print 'Show "%s" on helium display (DISP1).' % f
        return ret
        
    def __setStepperMotorPosition(self, pos):
        '''sets the stepper motor to a specific position. Only for testing Needlevalve.'''
        ret = self.__device.ask('G%i' % pos)
        print 'Set the Needle Valve to position %i.' % pos
        return ret
        
    def __setChanneltoSlow(self, n):
        '''sets one of the three meassurement channels to measurement slow'''
        ret = self.__device.ask('S%i' % n)
        print 'Set the measurement mode of channel %i to slow.' % n
        return ret
        
    def __setChanneltoFast(self, n):
        '''sets one of the three meassurement channels to measurement fast'''
        ret = self.__device.ask('T%i' % n)
        print 'Set the measurement mode of channel %i to fast.' % n
        return ret
        
        
    # ### System Commands (recognised only after correct SystemLockState) ### #
    
    def __loadRAM(self, n):
        '''loads binary into RAM. n=2 loads only 2kB, n=8 loads full 8kB.'''
        self.__device.write('Y%i' % n)
        print 'Loaded %ikB of data from binary to RAM.' % n
        
    def __dumpRAM(self, n):
        '''dumps RAM into binary. n=2 dumps only 2kB, n=8 dumps full 8kB.'''
        self.__device.write('Z%i' % n)
        print 'Dumped %ikB of data from RAM to binary.' % n
        
    def __changeIsobusAdress(self, adress):
        '''changes the isobus adress from the device'''
        self.__device.write('!%i' % adress)
        self.__device.changeAdress(adress)
        print 'Set the Isobus Adress of device to %i.' % adress
        
        
    # ### important commands for controlling
    
    def getState(self, decode = True):
        '''gives back the device State, if decode == True, the state is interpreted fully'''
        state = self.__getDeviceState()
        cha = map(int, state[1:4]) # gets channel usage
        sta = map(int, (state[5:7], state[7:9], state[9:11], state[12:14]), (16, 16, 16, 16)) # gets states
        for i in range(len(sta)):
            sta[i] = bin(sta[i])[2:].zfill(8)
        if decode:
            for i in range(len(sta)):
                sta[i] = map(int, sta[i])
            for i in range(len(cha)):
                cha[i] = self.__dictChannelUsage[cha[i]]
            for i in range(len(sta)-1):
                sta[i][0] = ' Pre-Pulse Current ON;' if sta[i][0] else ''
                sta[i][1] = ' ALARM Requested;' if sta[i][1] else ''
                sta[i][2] = ' Fill-level LOW;' if sta[i][2] else ''
                sta[i][5] = ' Helium probe in SLOW rate;' if sta[i][5] else ''
                sta[i][6] = ' Helium probe in FAST rate;' if sta[i][6] else ''
                sta[i][7] = ' Helium probe Current ON;' if sta[i][7] else ''
                if sta[i][3:5] == [0,0]:
                    sta[i][3:5] = [' END FILL;', '']
                elif sta[i][3:5] == [1,0]:
                    sta[i][3:5] = [' NOT FILLING;', '']
                elif sta[i][3:5] == [0,1]:
                    sta[i][3:5] = [' FILLING;', '']
                else:
                    sta[i][3:5] = [' START FILLING;', '']
            sta[3][0] = ' Relay 4 ACTIVE;' if sta[3][0] else ''
            sta[3][1] = ' Relay 3 ACTIVE;' if sta[3][1] else ''
            sta[3][2] = ' Relay 2 ACTIVE;' if sta[3][2] else ''
            sta[3][3] = ' Relay 1 ACTIVE;' if sta[3][3] else ''
            sta[3][4] = ' Alarm is SILENT;' if sta[3][4] else ''
            sta[3][5] = ' Alarm is ON (maybe silenced);' if sta[3][5] else ''
            sta[3][6] = ' Alarm sound is ON;' if sta[3][6] else ''
            sta[3][7] = ' in SHUT-DOWN state;' if sta[3][7] else ''
            ret = 'state:\n CHA1: %s, %s\n CHA2: %s, %s\n CHA3: %s, %s\n RELAY: %s' % (cha[0], ''.join(sta[0]), cha[1], ''.join(sta[1]), cha[2], ''.join(sta[2]), ''.join(sta[3]))
            return ret
        else:
            ret = 'state:\n CHA1: %i, %s;\n CHA2: %i, %s;\n CHA3: %i, %s;\n RELAY: %s' % (cha[0], ''.join(sta[0]), cha[1], ''.join(sta[1]), cha[2], ''.join(sta[2]), ''.join(sta[3]))
            return ret
        
    def checkCommunication(self):
        '''asks the device for Version, raises ERROR when no answer returns.'''
        ret = self.__getVersion()
        if len(ret) == 0:
            raise IOError('Could not contact the ILM200!')
        else:
            return ret
        
    def setControlState(self, state):
        '''sets the device control state to 'LOCAL', 'REMOTE+LOCKED', 'REMOTE+UNLOCKED' '''
        self.__setControlState(state)
        
    def getHeliumLevel(self):
        '''returns the measured Helium level'''
        return float(self.__getParameter('CHA1LVL'))/10.0
    
    def getNitrogenLevel(self):
        '''returns the measured Nitrogen level'''
        return float(self.__getParameter('CHA2LVL'))/10.0
    
    def getCha1Current(self):
        '''returns the measured Current from channel 1'''
        return self.__getParameter('CHA1CUR')
    
    def getCha2Current(self):
        '''returns the measured Current from channel 2'''
        return self.__getParameter('CHA2CUR')
    
    def getValvePosition(self):
        '''returns the current Needle-Valve position'''
        return self.__getParameter('VALVEPOS')
    
    def setHeliumProbeSpeed(self, speed):
        '''sets the Helium probe speed to 'FAST' or 'SLOW'. ILM has to be in REMOTE '''
        if speed == 'FAST':
            self.__setChanneltoFast(1)
            time.sleep(6)
            print 'Changed Helium probe speed to FAST.'
        else:
            self.__setChanneltoSlow(1)
            print 'Changed Helium probe speed to SLOW.'
            
        


class ITC503(object):
    '''Wrapper class for ITC503'''
    
    def __init__(self, port):
        self.__device = OxfordIsobusDevice(port, 1) #port should be COM port, 1 is standart device adress of ITC503
        
        # Dictionaries to convert Strings into Control parameters
        self.__dictControlState = {'LOCAL+LOCKED': 0, 'REMOTE+LOCKED': 1, 'LOCAL+UNLOCKED': 2, 'REMOTE+UNLOCKED': 3}
        self.__dictControlStateInv = dict([v,k] for k,v in self.__dictControlState.iteritems())
        self.__dictParameter = {'SET_TEMP': 0, 'TEMP1': 1, 'TEMP2': 2, 'TEMP3': 3, 'TEMP_ERR': 4, 'HEAT_OP': 5, 'HEAT_OP_V': 6, 'GAS_OP': 7, 'PROP_BAND': 8, 'INT_ACT_TIME': 9, 'DER_ACT_TIME': 10, 'CHA1F': 11, 'CHA2F': 12, 'CHA3F': 13}
        self.__dictSysLockState = {'LOCKED': 0, '!UNLOCKED': 1, 'SLEEP': 1234, 'WAKEUP': 4321, 'UNLOCK_LYZ': 9999}
        self.__dictAutoManualState = {'HEATMAN_GASMAN': 0, 'HEATAUTO_GASMAN': 1, 'HEATMAN_GASAUTO': 2, 'HEATAUTO_GASAUTO': 3}
        self.__dictAutoManualStateInv = dict([v,k] for k,v in self.__dictAutoManualState.iteritems())
        #self.__funcProportionalBand = lambda x: float('%.3f' % (58.67807*np.exp(-x/23.89865) + 3.5)) # obtained from "exponential decay" fit of oxford data
        #self.__funcDerivativeActionTime = lambda x: float('%.1f' % (0.0))
        #self.__funcIntegralActionTime = lambda x: float('%.1f' % (2.5 + (0.2-2.5)/(1.0+(x/35.0)**85.0))) # obtained from "Logistic" Fit of oxford data
        self.__tabPIDParameter = [[1.55, 3.0, 4.2, 6.0, 10.0, 20.0, 50.0, 77.0, 100.0, 200.0, 300.0], \
                                  [60.0, 50.0, 50.0, 50.0, 50.0, 50.0, 5.0, 3.5, 3.5, 3.5, 3.5], \
                                  [0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 2.5, 2.5, 2.5, 2.5, 2.5], \
                                  [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]] # [T, P, I, D]
        self.__funcProportionalBand = lambda x: np.interp(x, self.__tabPIDParameter[0], self.__tabPIDParameter[1])
        self.__funcIntegralActionTime = lambda x: np.interp(x, self.__tabPIDParameter[0], self.__tabPIDParameter[2])
        self.__funcDerivativeActionTime = lambda x: np.interp(x, self.__tabPIDParameter[0], self.__tabPIDParameter[3])
        
        
    # ### Monitor Commands (always recognised) ### #
    
    def __setControlState(self, f):
        '''sets the control state of the device'''
        ret = self.__device.ask('C%i' % self.__dictControlState[f])
        print 'Set control state to "%s".' % f
        return ret
        
    def __setCommunicationProtocol(self, f):
        '''defines how the device answers (only 0 and 2 are available)'''
        self.__device.write('Q%i' % f)
        print 'Set communication portocoll to "%s".' % f
        
    def __getParameter(self, f):
        '''asks for Parameter to read out'''
        ret = self.__device.ask('R%i' % self.__dictParameter[f])
        return float(ret.lstrip('R'))
    
    def __setSystemLockState(self, f):
        '''sets the system locke state which gives acces to system commands'''
        ret = self.__device.ask('U%i' % self.__dictSysLockState[f])
        print 'Set system lock state to "%s".' % f
        return ret
        
    def __getVersion(self):
        '''asks for the Device and Software Version'''
        ret = self.__device.ask('V')
        return ret
    
    def __setOutputWaitingTime(self, time):
        '''sets the waiting time in miliseconds between character sending of device'''
        ret = self.__device.ask('W%i' % time)
        print 'Set output waiting time to %i ms.' % time
        return ret
        
    def __getDeviceState(self):
        '''reads out the state of the device'''
        ret = self.__device.ask('X')
        return ret
    
    
    # ### Control Commands (recognised only in REMOTE mode) ### #
    
    def __setAutoManualMode(self, f):
        '''sets the auto/manual control mode for heater and gasflow'''
        ret = self.__device.ask('A%i' % self.__dictAutoManualState[f])
        print 'Set the Gas-/Heater-controll mode to "%s".' % f
        return ret
        
    def __setDerivativeActionTime(self, time):
        '''sets the derivative value of the PID control in minutes ranging from 0 to 273'''
        ret = self.__device.ask('D%i' % time)
        print 'Set the derivative action time to %i min.' % time
        return ret
    
    def __setDisplaySrc(self, f):
        '''defines the Parameter which is shown on Display'''
        ret = self.__device.ask('F%i' % self.__dictParameter[f])
        print 'Show "%s" on display.' % f
        return ret
        
    def __setGasFlow(self, val):
        '''sets the gasflow in x.1% resolution when in GASMANual mode'''
        ret = self.__device.ask('G%.1f' % val)
        print 'Set the gas flow to val %.1f %%.' % val
        return ret
        
    def __setHeaterControlSensor(self, n):
        '''sets the temperature sensor (1-3) which is used to automatic control the heater'''
        ret = self.__device.ask('H%i' % n)
        print 'Set temperature sensor %i to heater control sensor.' % n
        return ret
        
    def __setIntegralActionTime(self, time):
        '''sets the integral value of the PID control in 0.1 minute resolution ranging from 0 to 140'''
        ret = self.__device.ask('I%.1f' % time)
        print 'Set the integral action time to %.1f min.' % time
        return ret
        
    def __setAutoPID(self, f):
        '''ENABLES - 1/ DISABLES - 0 the the use of internal learned AUTO PID'''
        ret = self.__device.ask('L%i' % f)
        if f == 0:
            print 'Auto PID DISABLED.'
        else:
            print 'Auto PID ENABLED.'
        return ret
            
    def __setHeaterMaximumVolts(self, U):
        '''sets the heaters maximum Volt limit in 0.1 V resolution from 0 V to 40 V'''
        ret = self.__device.ask('M%.1f' % U)
        print 'Set heater maximum voltage to %.1f V.' % U
        return ret
        
    def __setHeaterOutput(self, val):
        '''sets the heater ouput to a percentage of given heater maximum. Only in MANUAL mode. 0.1 % resolution from 0.0 till 99.9'''
        ret = self.__device.ask('O%.1f' % val)
        print 'Set the heater output to %.1f %% of heater voltage limit.' % val
        return ret
        
    def __setProportionalBand(self, temp):
        '''sets the propotional value from PID control in 0.001 K resolution'''
        ret = self.__device.ask('P%.3f' % temp)
        print 'Set the proportional band value to %.3f K.' % temp
        return ret
        
    def __setSweepState(self, f):
        '''STARTs - 1 or STOPs - 0 a sweep'''
        ret = self.__device.ask('S%i' % f)
        if f == 0:
            print 'Stopped a sweep.'
        else:
            print 'Started a sweep.'
        return ret
            
    def __setTemperature(self, temp):
        '''sets the set point temperature in K'''
        ret = self.__device.ask('T%f' % temp)
        print 'Set the set point temperature to %f K.' %temp
        return ret
        
        
    # ### System Commands (recognised only after correct SystemLockState) ### #
    
    def __loadRAM(self, n):
        '''loads binary into RAM. n=2 loads only 2kB, n=8 loads full 8kB.'''
        self.__device.write('Y%i' % n)
        print 'Loaded %ikB of data from binary to RAM.' % n
        
    def __dumpRAM(self, n):
        '''dumps RAM into binary. n=2 dumps only 2kB, n=8 dumps full 8kB.'''
        self.__device.write('Z%i' % n)
        print 'Dumped %ikB of data from RAM to binary.' % n
        
    def __storeRAMtoEEPROM(self):
        '''stores the entire RAM in EEPROM memory, in firmware 1.07 or later.'''
        ret = self.__device.ask('~')
        print 'Stored the RAM into EEPROM'
        return ret
        
    def __changeIsobusAdress(self, adress):
        '''changes the isobus adress from the device'''
        self.__device.write('!%i' % adress)
        self.__device.changeAdress(adress)
        print 'Set the Isobus Adress of device to %i.' % adress
        
        
    # ### specialist commands !!!NOT IMPLEMENTED NOW!!!
        
        
    # ### important commands for controlling
    
    def getState(self, decode = True):
        '''gives back the device State, if decode == True, the state is interpreted fully'''
        state = self.__getDeviceState()
        sta = map(int, (state[1], state[3], state[5], state[7:9], state[10], state[12])) # gets the values
        if decode:
            sta[0] = ' system state = 0;'
            sta[1] = ' AUTO/MANUAL state = %s;' % self.__dictAutoManualStateInv[sta[1]]
            sta[2] = ' LOCAL/REMOTE state = %s;' % self.__dictControlStateInv[sta[2]]
            if sta[3] == 0:
                sta[3] = ' No sweep running;'
            elif np.mod(sta[3], 2) == 0:
                sta[3] = ' Holding at step %i;' % stat[3]/2
            else:
                sta[3] = ' Sweeping to step %i;' % stat[3]/2 + 1
            sta[4] = ' Temperatur sensor for Control is %i;' % sta[4]
            sta[5] = ' AUTO PID is ENABLED;' if sta[5] == 1 else ' AUTO PID is DISABLED;'
            ret = 'state:\n%s' % ('\n'.join(sta))
            return ret
        else:
            ret = 'state:\n system state: %i;\n AUTO/MAN: %i;\n LOCAL/REMOTE: %i;\n sweep state: %i;\n heater control sensor: %i;\n auto PID state: %i' % (sta[0], sta[1], sta[2], sta[3], sta[4], sta[5])
            return ret
        
    def checkCommunication(self):
        '''asks the device for Version, raises ERROR when no answer returns.'''
        ret = self.__getVersion()
        if len(ret) == 0:
            raise IOError('Could not contact the ITC503!')
        else:
            return ret
        
    def setControlState(self, state):
        '''sets the device control state to 'LOCAL+LOCKED', 'REMOTE+LOCKED', 'LOCAL+UNLOCKED', 'REMOTE+UNLOCKED' '''
        self.__setControlState(state)
        
    def getSetpointTemperature(self):
        '''returns the actual setpoint temperature'''
        return self.__getParameter('SET_TEMP')
        
    def getTemperatur1(self):
        '''returns the measured temperatur of sensor 1'''
        return self.__getParameter('TEMP1')
    
    def getTemperatur2(self):
        '''returns the measured temperatur of sensor 2'''
        return self.__getParameter('TEMP2')
    
    def getTemperatur3(self):
        '''returns the measured temperatur of sensor 3'''
        return self.__getParameter('TEMP3')
    
    def getTemperatureError(self):
        '''returns the difference between measured temperature and setpoint temperature'''
        return self.__getParameter('TEMP_ERR')
    
    def getHeaterOutput(self, volt = True):
        '''returns the current heater output in Volt if volt==True else in percentage of maximum heater limit'''
        if volt:
            return self.__getParameter('HEAT_OP_V')
        else:
            return self.__getParameter('HEAT_OP')
    
    def getGasflow(self):
        '''returns the current gasflow in percentage'''
        return self.__getParameter('GAS_OP')
    
    def getP(self):
        '''returns the set proportional band value'''
        return self.__getParameter('PROP_BAND')
    
    def getI(self):
        '''returns the set integrational action time value'''
        return self.__getParameter('INT_ACT_TIME')
    
    def getD(self):
        '''return the set derivative action time value'''
        return self.__getParameter('DER_ACT_TIME')
    
    def setHeaterAUTO(self, auto):
        '''sets the heater from AUTO - 1 to MANUAL - 0 mode or the other way round'''
        state = self.__getDeviceState()
        if int(state[3]) in [0, 1]:
            if auto:
                self.__setAutoManualMode('HEATAUTO_GASMAN')
            else:
                self.__setAutoManualMode('HEATMAN_GASMAN')
        else:
            if auto:
                self.__setAutoManualMode('HEATAUTO_GASAUTO')
            else:
                self.__setAutoManualMode('HEATMAN_GASAUTO')
                
    def setGasAUTO(self, auto):
        '''sets the gas flow from AUTO - 1 to MANUAL - 0 mode or other way round'''
        state = self.__getDeviceState()
        if int(state[3]) in [0, 2]:
            if auto:
                self.__setAutoManualMode('HEATMAN_GASAUTO')
            else:
                self.__setAutoManualMode('HEATMAN_GASMAN')
        else:
            if auto:
                self.__setAutoManualMode('HEATAUTO_GASAUTO')
            else:
                self.__setAutoManualMode('HEATAUTO_GASMAN')
                
    def setGasFlow(self, val):
        '''sets the gas flow valve opening percentage in (only) manual mode'''
        self.__setGasFlow(val)
        
    def setHeaterControlSensor(self, n):
        '''changes the sensor (1/2/3) for automatic control of heater'''
        self.__setHeaterControlSensor(n)
        
    def turnOnAutoPID(self):
        '''turns on the internal PID tables (if there are any)'''
        self.__setAutoPID(1)
        
    def turnOffAutoPID(self):
        '''turns the internal PID tables off'''
        self.__setAutoPID(0)
        
    def setMaximumHeaterLimit(self, U):
        '''sets the maximum heater limit between 0 and 40 V in 0.1 V resolution'''
        self.__setHeaterMaximumVolts(U)
        
    def setHeaterOutput(self, val):
        '''sets the heater output voltage in percent of maximum limit from 0.0 till 99.9 %. Only in HEATMANual mode'''
        self.__setHeaterOutput(val)
        
    def setTemperature(self, temp):
        '''sets the setpoint temperature and also sends the PID parameters (given by Oxford calibration) to device'''
        print 'Set temperature to %f K.' % temp 
        self.__setProportionalBand(self.__funcProportionalBand(temp))
        self.__setIntegralActionTime(self.__funcIntegralActionTime(temp))
        self.__setDerivativeActionTime(self.__funcDerivativeActionTime(temp))
        self.__setTemperature(temp)
        
        
        
        
class LambdaController(object):
    '''Wrapper class for Lambda Controller'''
    
    def __init__(self, port):
        self.__device = OxfordIsobusDevice(port, 3) #port should be COM port, 3 is standart device adress of Lambda Controller
        
        # Dictionaries to convert Strings into Control parameters
        self.__dictControlState = {'STANDBY': 0, 'ACTIVE': 3}
        self.__dictControlStateInv = dict([v,k] for k,v in self.__dictControlState.iteritems())
        self.__dictParameter = {'SENS1_PRESS': 1, 'SENS2_TEMP': 2, 'SENS3': 3, 'VALVE_POS': 7, 'CHA1F': 11, 'CHA2F': 12, 'CHA3F': 13}
        self.__dictSysLockState = {'LOCKED': 0, '!UNLOCKED': 1, 'SLEEP': 1234, 'WAKEUP': 4321, 'UNLOCK_LYZ': 9999}
        
        
    # ### Monitor Commands (always recognised) ### #
    
    def __setControlState(self, f):
        '''sets the control state of the device'''
        ret = self.__device.ask('C%i' % self.__dictControlState[f])
        print 'Set control state to "%s".' % f
        return ret
        
    def __setCommunicationProtocol(self, f):
        '''defines how the device answers (only 0 and 2 are available)'''
        self.__device.write('Q%i' % f)
        print 'Set communication portocoll to "%s".' % f
        
    def __getParameter(self, f):
        '''asks for Parameter to read out'''
        ret = self.__device.ask('R%i' % self.__dictParameter[f])
        return float(ret.lstrip('R'))
    
    def __setSystemLockState(self, f):
        '''sets the system locke state which gives acces to system commands'''
        ret = self.__device.ask('U%i' % self.__dictSysLockState[f])
        print 'Set system lock state to "%s".' % f
        return ret
        
    def __getVersion(self):
        '''asks for the Device and Software Version'''
        ret = self.__device.ask('V')
        return ret
    
    def __setOutputWaitingTime(self, time):
        '''sets the waiting time in miliseconds between character sending of device'''
        ret = self.__device.ask('W%i' % time)
        print 'Set output waiting time to %i ms.' % time
        return ret
        
    def __getDeviceState(self):
        '''reads out the state of the device'''
        ret = self.__device.ask('X')
        return ret
    
    
    # ### Control Commands (recognised only in REMOTE mode) ### #
    
    def __setDisplaySrc(self, f):
        '''defines the Parameter which is shown on Display'''
        ret = self.__device.ask('F%i' % self.__dictParameter[f])
        print 'Show "%s" on display.' % f
        return ret
    
    def __setGasFlow(self, val):
        '''sets the gas flow through the lambda fridge by opening needle valve to val'''
        ret = self.__device.ask('G%i' % int(val*10))
        print 'Set the gas flow to "%i"%%.' % val
        return ret
        
        
    # ### System Commands (recognised only after correct SystemLockState) ### #
    
    def __loadLinearizer(self, n):
        '''loads one of three customisable non-volatile data tables with linearisation for particular sensor'''
        self.__device.write('L%i' % n)
        print 'Loaded Lineariser table %i.' % n
    
    def __loadRAM(self, n):
        '''loads binary into RAM. n=2 loads only 2kB, n=8 loads full 8kB.'''
        self.__device.write('Y%i' % n)
        print 'Loaded %ikB of data from binary to RAM.' % n
        
    def __dumpRAM(self, n):
        '''dumps RAM into binary. n=2 dumps only 2kB, n=8 dumps full 8kB.'''
        self.__device.write('Z%i' % n)
        print 'Dumped %ikB of data from RAM to binary.' % n
        
    def __changeIsobusAdress(self, adress):
        '''changes the isobus adress from the device'''
        self.__device.write('!%i' % adress)
        self.__device.changeAdress(adress)
        print 'Set the Isobus Adress of device to %i.' % adress
        
        
    # ### specialist commands !!!NOT IMPLEMENTED NOW!!!
        
        
    # ### important commands for controlling
    
    def getState(self, decode = True):
        '''gives back the device State, if decode == True, the state is interpreted fully'''
        state = self.__getDeviceState()
        sta = map(int, (state[1], state[3], state[5], state[7], state[9])) # gets the values
        if decode:
            sta[0] = ' system state = 0;'
            sta[1] = ' activity state = 0;' if sta[1] == 0 else 'activity state = Valve initialize;'
            sta[2] = ' ACTIVE/INACTIVE state = %s;' % self.__dictControlStateInv[sta[2]]
            sta[3] = ' no sweep running;'
            sta[4] = ' sensor state = 1;'
            ret = 'state:\n%s' % ('\n'.join(sta))
            return ret
        else:
            ret = 'state:\n system state: %i;\n activity state: %i;\n STANDBY state: %i;\n sweep state: %i;\n sensor state: %i' % (sta[0], sta[1], sta[2], sta[3], sta[4])
            return ret
        
    def checkCommunication(self):
        '''asks the device for Version, raises ERROR when no answer returns.'''
        ret = self.__getVersion()
        if len(ret) == 0:
            raise IOError('Could not contact the Lambda Controller!')
        else:
            return ret
        
    def setControlState(self, state):
        '''sets the device control state to 'STANDBY', 'ACTIVE' '''
        self.__setControlState(state)
        
    def getPressure(self):
        '''returns the current pressure in mbar'''
        return float(self.__getParameter('SENS1_PRESS'))/10.0
        
    def getTemperature(self):
        '''returns the measured temperature'''
        return float(self.__getParameter('SENS2_TEMP'))/100.0
    
    def getValvePosition(self):
        '''returns the current needle valve position in units 0.1% of travel'''
        return float(self.__getParameter('VALVE_POS'))/10.0
    
    def setGasFlow(self, val):
        '''set the gas flow of the Lambda fridge in units of 0.1% of needle valve travel range'''
        self.__setGasFlow(val)
        
        
        
        
class IPS120(object):
    '''Wrapper class for IPS120-10-R'''
    
    def __init__(self, port):
        self.__device = OxfordIsobusDevice(port, 2) #port should be COM port, 2 is standart device adress of IPS120-10-R
        #self.__device.write('Q6') #give back values with enhanced resolution
        
        # Dictionaries to convert Strings into Control parameters
        self.__dictControlState = {'LOCAL+LOCKED': 0, 'REMOTE+LOCKED': 1, 'LOCAL+UNLOCKED': 2, 'REMOTE+UNLOCKED': 3}
        self.__dictParameter = {'CUR_OUTPUT': 0, 'VOLT_PS': 1, 'CUR_MAG': 2, 'CUR_SETPOINT': 5, 'CUR_SWEEPRATE': 6, 'FIELD_OUTPUT': 7, 'FIELD_SETPOINT': 8, 'FIELD_SWEEPRATE': 9, 'DAC_OFFSET': 10,\
                                'CHA1F': 11, 'CHA2F': 12, 'CHA3F': 13, 'VOLT_LIMIT': 15, 'CUR_MAG_PERSISTENT': 16, 'CUR_TRIP': 17, 'FIELD_PERSISTENT': 18, 'FIELD_TRIP': 19, 'CUR_SWITCH_HEATER': 20,\
                                'CUR_LIMIT_NEG': 21, 'CUR_LIMIT_POS': 22, 'LEAD_RESIST': 23, 'INDUCTANCE_MAG': 24}
        self.__dictSysLockState = {'LOCKED': 0, '!UNLOCKED': 1, 'SLEEP': 1234, 'WAKEUP': 4321, 'UNLOCK_LYZ': 9999}
        self.__dictSwitchHeater = {'OFF': 0, 'ON': 1, 'ON_NO_CHECK': 2}
        self.__dictMode = {'AMPS+FAST': 0, 'TESLA+FAST': 1, 'AMPS+SLOW': 4, 'TESLA+SLOW': 5, 'AMPS': 8, 'TESLA': 9}
        self.__dictPolarity = {'NO_ACT': 0, 'SET_POS_CUR': 1, 'SET_NEG_CUR': 2, 'SWAP_POL': 4}
        self.__dictSysStateM = {0: 'NORMAL', 1: 'QUENCHED', 2: 'OVER_HEATED', 4: 'WARMING_UP', 8: 'FAULT'}
        self.__dictSysStateN = {0: 'NORMAL', 1: 'POS_VOLT_LIMIT_REACHED', 2: 'NEG_VOLT_LIMIT_REACHED', 4: 'NEG_CUR_LIMIT_EXCEEDED', 8: 'POS_CUR_LIMIT_EXCEEDED'}
        self.__dictActivity = {0: 'HOLD', 1: 'TO_SETPOINT', 2: 'TO_ZERO', 4: 'CLAMPED'}
        self.__dictActivityInv = dict([v,k] for k,v in self.__dictActivity.iteritems())
        self.__dictControlStateInv = {0: 'LOCAL+LOCKED', 1: 'REMOTE+LOCKED', 2: 'LOCAL+UNLOCKED', 3: 'REMOTE+UNLOCKED', 4: 'AUTO_RUNDOWN', 5: 'AUTO_RUNDOWN', 6: 'AUTO_RUNDOWN', 7: 'AUTO_RUNDOWN'}
        self.__dictSwitchHeaterInv = {0: 'OFF_AT_ZERO', 1: 'ON', 2: 'OFF_AT_FIELD', 5: 'FAULT', 8: 'NO_SWITCH_FITTED'}
        self.__dictModeM = {0: 'display: AMPS, magnet sweep: FAST', 1: 'display: TESLA, magnet sweep: FAST', 4: 'display: AMPS, magnet sweep: SLOW', 5: 'display: TESLA, magnet sweep: SLOW'}
        self.__dictModeN = {0: 'AT_REST', 1: 'SWEEPING', 2: 'SWEEP_LIMITING', 3: 'SWEEPING+SWEEP_LIMITING'}
        self.__dictPolarityM = {0: 'desired: POSITIVE, magnet: POSITIVE, commanded: POSITIVE', 1: 'desired: POSITIVE, magnet: POSITIVE, commanded: NEGATIVE', 2: 'desired: POSITIVE, magnet: NEGATIVE, commanded: POSITIVE', 3: 'desired: POSITIVE, magnet: NEGATIVE, commanded: NEGATIVE',\
                                4: 'desired: NEGATIVE, magnet: POSITIVE, commanded: POSITIVE', 5: 'desired: NEGATIVE, magnet: POSITIVE, commanded: NEGATIVE', 6: 'desired: NEGATIVE, magnet: NEGATIVE, commanded: POSITIVE', 7: 'desired: NEGATIVE, magnet: NEGATIVE, commanded: NEGATIVE'}
        self.__dictPolarityN = {1: 'NEGATIVE_CONTACT_CLOSED', 2: 'POSITIVE_CONTACT_CLOSED', 3: 'BOTH_CONTACTS_OPEN', 4: 'BOTH_CONTACTS_CLOSED'}
        
        
    # ### Monitor Commands (always recognised) ### #
    
    def __setControlState(self, f):
        '''sets the control state of the device'''
        ret = self.__device.ask('C%i' % self.__dictControlState[f])
        print 'Set control state to "%s".' % f
        return ret
        
    def __setCommunicationProtocol(self, f):
        '''defines how the device answers (0 - Normal, 2 - \r\n, 4 - extended Resolution, 6 - extended resolution + \r\n)'''
        self.__device.write('Q%i' % f)
        print 'Set communication portocoll to "%s".' % f
        
    def __getParameter(self, f):
        '''asks for Parameter to read out'''
        ret = self.__device.ask('R%i' % self.__dictParameter[f])
        return float(ret.lstrip('R'))
    
    def __setSystemLockState(self, f):
        '''sets the system locke state which gives acces to system commands'''
        ret = self.__device.ask('U%i' % self.__dictSysLockState[f])
        print 'Set system lock state to "%s".' % f
        return ret
        
    def __getVersion(self):
        '''asks for the Device and Software Version'''
        ret = self.__device.ask('V')
        return ret
    
    def __setOutputWaitingTime(self, time):
        '''sets the waiting time in miliseconds between character sending of device'''
        ret = self.__device.ask('W%i' % time)
        print 'Set output waiting time to %i ms.' % time
        return ret
        
    def __getDeviceState(self):
        '''reads out the state of the device'''
        ret = self.__device.ask('X')
        return ret
    
    
    # ### Control Commands (recognised only in REMOTE mode) ### #
    
    def __setActivity(self, f):
        '''starts different activitys of the device'''
        ret = self.__device.ask('A%i' % self.__dictActivityInv[f])
        print 'Performs "%s" mode.' % f
        return ret
    
    def __setDisplaySrc(self, f):
        '''defines the Parameter which is shown on Display'''
        ret = self.__device.ask('F%i' % self.__dictParameter[f])
        print 'Show "%s" on display.' % f
        return ret
        
    def __setSwitchHeater(self, f):
        '''controls the switch heater'''
        ret = self.__device.ask('H%i' % self.__dictSwitchHeater[f])
        print 'Set the switch heater to %s mode.' % f
        return ret
        
    def __setCurrentSetpoint(self, i):
        '''sets the current setpoint in 0.0001 A resolution'''
        ret = self.__device.ask('I%.4f' % i)
        print 'Set the current setpoint to %.4f A.' % i
        return ret
        
    def __setFieldSetpoint(self, b):
        '''sets the field setpoint in 0.0001 T resolution'''
        ret = self.__device.ask('J%.4f' % b)
        print 'Set the field setpoint to %.4f T.' % b
        return ret
        
    def __setMode(self, f):
        '''sets the CURRENT or FIELD mode and SLOW or FAST sweep mode'''
        ret = self.__device.ask('M%i' % self.__dictMode[f])
        print 'Set the device to "%s" mode.' % f
        return ret
        
    def __setPolarity(self, f):
        '''sets the polarity of the magnet'''
        ret = self.__device.ask('P%i' % self.__dictPolarity[f])
        print 'Set the polarity of the magnet to "%s".' % f
        return ret
            
    def __setCurrentSweepRate(self, sr):
        '''sets the current sweep rate in A/min'''
        ret = self.__device.ask('S%f' % sr)
        print 'Set the current sweep rate to %f A/min.' % sr
        return ret
        
    def __setFieldSweepRate(self, sr):
        '''sets the field sweep rate in T/min (0.0001 resolution)'''
        ret = self.__device.ask('T%.4f' % sr)
        print 'Set the field sweep rate to %.4f T/min.' % sr
        return ret
        
        
    # ### System Commands (recognised only after correct SystemLockState) ### #
    
    def __loadRAM(self, n):
        '''loads binary into RAM. n=2 loads only 2kB, n=8 loads full 8kB.'''
        self.__device.write('Y%i' % n)
        print 'Loaded %ikB of data from binary to RAM.' % n
        
    def __dumpRAM(self, n):
        '''dumps RAM into binary. n=2 dumps only 2kB, n=8 dumps full 8kB.'''
        self.__device.write('Z%i' % n)
        print 'Dumped %ikB of data from RAM to binary.' % n
        
    def __storeRAMtoEEPROM(self):
        '''stores the entire RAM in EEPROM memory.'''
        ret = self.__device.ask('~')
        print 'Stored the RAM into EEPROM'
        return ret
        
    def __changeIsobusAdress(self, adress):
        '''changes the isobus adress from the device'''
        self.__device.write('!%i' % adress)
        self.__device.changeAdress(adress)
        print 'Set the Isobus Adress of device to %i.' % adress
        
        
    # ### specialist commands !!!NOT IMPLEMENTED NOW!!!
        
        
    # ### important commands for controlling
    
    def getState(self, decode = True):
        '''gives back the device State, if decode == True, the state is interpreted fully'''
        state = self.__getDeviceState()
        sta = map(int, (state[1], state[2], state[4], state[6], state[8], state[10], state[11], state[13], state[14])) # gets the values
        ret = []
        if decode:
            ret.append(' system state: %s, %s;' % (self.__dictSysStateM[sta[0]], self.__dictSysStateN[sta[1]]))
            ret.append(' activity: %s;' % self.__dictActivity[sta[2]])
            ret.append(' LOCAL/REMOTE state = %s;' % self.__dictControlStateInv[sta[3]])
            ret.append(' switch heater: %s;' % self.__dictSwitchHeaterInv[sta[4]])
            ret.append(' mode: %s, %s' % (self.__dictModeM[sta[5]], self.__dictModeN[sta[6]]))
            ret.append(' polarity: %s, %s' % (self.__dictPolarityM[sta[7]], self.__dictPolarityN[sta[8]]))
            ret = 'state:\n%s' % ('\n'.join(ret))
            return ret
        else:
            ret = 'state:\n system state: %i, %i;\n activity: %i;\n LOCAL/REMOTE: %i;\n switch heater: %i;\n mode: %i, %i;\n polarity: %i, %i' % (sta[0], sta[1], sta[2], sta[3], sta[4], sta[5], sta[6], sta[7], sta[8])
            return ret
        
    def checkCommunication(self):
        '''asks the device for Version, raises ERROR when no answer returns.'''
        ret = self.__getVersion()
        if len(ret) == 0:
            raise IOError('Could not contact the ITC503!')
        else:
            return ret
        
    def setControlState(self, state):
        '''sets the device control state to 'LOCAL+LOCKED', 'REMOTE+LOCKED', 'LOCAL+UNLOCKED', 'REMOTE+UNLOCKED' '''
        self.__setControlState(state)
        
    def getCurrentOutput(self):
        '''returns the actual current output in A'''
        return self.__getParameter('CUR_OUTPUT')
    
    def getNegativeCurrentLimit(self):
        '''returns the most negative safe current limit in A'''
        return self.__getParameter('CUR_LIMIT_NEG')
    
    def getPositiveCurrentLimit(self):
        '''returns the most positive safe current limit in A'''
        return self.__getParameter('CUR_LIMIT_POS')
        
    def getCurrentSetpoint(self):
        '''returns the actual current setpoint in A'''
        return self.__getParameter('CUR_SETPOINT')
    
    def getCurrentSweepRate(self):
        '''returns the actual current sweep rate in A/min'''
        return self.__getParameter('CUR_SWEEPRATE')
    
    def getMagnetCurrent(self):
        '''returns the actual current in the magnet in A'''
        return self.__getParameter('CUR_MAG')
    
    def getPersistentMagnetCurrent(self):
        '''returns the actual persistent magnet current in A'''
        return self.__getParameter('CUR_MAG_PERSISTENT')
    
    def getTripCurrent(self):
        '''returns the actual trip current in A'''
        return self.__getParameter('CUR_TRIP')
    
    def getPowersupplyVoltage(self):
        '''returns the measured powersupply voltage in V'''
        return self.__getParameter('VOLT_PS')
    
    def getVoltageLimit(self):
        '''returns the actual voltage limit in V'''
        return self.__getParameter('VOLT_LIMIT')
    
    def getFieldOutput(self):
        '''returns the current field output of the magnet in T'''
        return self.__getParameter('FIELD_OUTPUT')
    
    def getPersistentField(self):
        '''returns the persistent field of the magnet in T'''
        return self.__getParameter('FIELD_PERSISTENT')
    
    def getTripField(self):
        '''returns the trip field in T'''
        return self.__getParameter('FIELD_TRIP')
    
    def getFieldSetpoint(self):
        '''returns the current field set point in T'''
        return self.__getParameter('FIELD_SETPOINT')
    
    def getFieldSweepRate(self):
        '''returns the current field sweep rate in T/min'''
        return self.__getParameter('FIELD_SWEEPRATE')
    
    def getSwitchHeaterCurrent(self):
        '''returns the switch heater current in mA'''
        return self.__getParameter('CUR_SWITCH_HEATER')
    
    def getLeadResistance(self):
        '''returns the lead resistance in mOhm'''
        return self.__getParameter('LEAD_RESIST')
    
    def getMagnetInductance(self):
        '''returns the magnet inductance in H(Henry)'''
        return self.__getParameter('INDUCTANCE_MAG')
        
    def holdValue(self):
        '''sets the Activity mode to HOLD'''
        self.__setActivity('HOLD')
        
    def gotoSetpoint(self):
        '''performs sweep to setpoint value'''
        self.__setActivity('TO_SETPOINT')
        
    def gotoZero(self):
        '''poerforms sweep to 0 value'''
        self.__setActivity('TO_ZERO')
        
    def clampPowersupplyOutput(self):
        '''clamps the powersupply output (like in powerup state?)'''
        self.__setActivity('CLAMPED')
        
    def turnOffSwitchHeater(self):
        '''turns off the switch heater'''
        self.__setSwitchHeater('OFF')
        
    def turnOnSwitchHeater(self, safe=True):
        '''turns on the switch heater if save=True (default), checks for problems'''
        if safe:
            self.__setSwitchHeater('ON')
        else:
            self.__setSwitchHeater('ON_NO_CHECK')
            
    def setCurrent(self, i):
        '''sets the current setpoint 0.0001 A resolution'''
        self.__setCurrentSetpoint(i)
        
    def setField(self, b):
        '''sets the field setpoint 0.0001 T resolution'''
        self.__setFieldSetpoint(b)
        
    def setControlParameter(self, f):
        '''sets the Control Parameter to AMPERE or TESLA'''
        if f == 'AMPS':
            self.__setMode('AMPS')
        else:
            self.__setMode('TESLA')
            
    def setSweepSpeed(self, f):
        '''sets the magnet sweep speed to SLOW or FAST'''
        tmp = self.__getDeviceState()[10]
        if f == 'SLOW':
            if tmp == 0 or tmp == 4:
                self.__setMode('AMPS+SLOW')
            else:
                self.__setMode('TESLA+SLOW')
        else:
            if tmp == 0 or tmp == 4:
                self.__setMode('AMPS+FAST')
            else:
                self.__setMode('TESLA+FAST')
                
    def setCurrentSweepRate(self, sr):
        '''sets the magnet current sweep rate 0.0001 A/min resolution?'''
        self.__setCurrentSweepRate(sr)
        
    def setFieldSweepRate(self, sr):
        '''sets the magnet field sweep rate 0.0001 T/min resolution'''
        self.__setFieldSweepRate(sr)
        
        
class OxfordControl(threading.Thread):
    '''Class which continuously controlls the Parameter of the Cryostat and saves them into a protocollfile, and controls the Cryostat'''
    
    def __init__(self):
        threading.Thread.__init__(self)
        self.__port = serial.Serial(port=0, baudrate=9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_TWO, timeout=10)
        self.__tasks = Queue.Queue()
        self.__answs = Queue.Queue()
        self.__stopRequest = threading.Event()
        self.__startTime = time.time()
        self.__ilmPars = [] #a list of the currently measured ILM parameters
        self.__itcPars = [] #a list of the currently measured ITC parameters
        self.__lcPars = [] #a list of the currently measured LC parameters
        self.__ipsPars = [] #a list of the currently measured IPS parameters
        self.__protocollFile = open('C:/Users/HLPH/Documents/Users/CryoProtocoll/oxford_protocoll_%s_%s.txt' % (self.__date(), self.__time()), mode = 'w')
        para = ['t', 'He', 'N2', 'T_1', 'T_2', 'T_3', 'T_sp', 'T_err', 'GF_t', 'U_heat', 'P_lamb', 'T_lamb', 'GF_lamb', 'I_out', 'I_sp', 'I_mag', 'I_pers', 'I_trip', 'B_out', 'B_sp', 'B_pers', 'B_trip', 'U_out', 'I_sh']
        self.__protocollFile.write('\t'.join(para)+'\n')
        unit = ['s', '%', '%', 'K', 'K', 'K', 'K', 'K', '%', 'V', 'mbar', 'K', '%', 'A', 'A', 'A', 'A', 'A', 'T', 'T', 'T', 'T', 'V', 'mA']
        self.__protocollFile.write('\t'.join(unit)+'\n')
        
        # ILM
        self.ilm = ILM200(self.__port)
        try:
            ret = self.ilm.checkCommunication()
            print 'Connected to ILM200: %s' % ret
        except:
            print 'Could not connect to ILM200.'
        
        # ITC
        self.itc = ITC503(self.__port)
        try:
            ret = self.itc.checkCommunication()
            print 'Connected to ITC503: %s' % ret
        except:
            print 'Could not connect to ITC503.'
            
        # Lambda Control
        self.lc = LambdaController(self.__port)
        try:
            ret = self.lc.checkCommunication()
            print 'Connected to Lambda Controller: %s' % ret
        except:
            print 'Could not connect to Lambda Controller.'
            
        # IPS
        self.ips = IPS120(self.__port)
        try:
            ret = self.ips.checkCommunication()
            print 'Connected to IPS120-10-R: %s' % ret
        except:
            print 'Could not connect to IPS120-10-R.'
            
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.setFieldSweepRate(0.3382)
        self.ips.setControlState('LOCAL+UNLOCKED')
        
            
    def run(self):
        '''performs an endless loop andd looks in the Queue if there's any additional task to do'''
        while 1:
            if self.__stopRequest.isSet():
                self.__port.close()
                break
            else:
                #loop part
                self.__ilmPars = self.__getILM() #getting all important parameters from ILM
                self.__itcPars = self.__getITC() #getting all important parameters from ITC
                self.__lcPars = self.__getLC() #getting all important parameters from LC
                self.__ipsPars = self.__getIPS() #getting all important parameters from IPS
                self.__writeProtocoll(self.__ilmPars, self.__itcPars, self.__lcPars, self.__ipsPars) #saving the parameter
                self.__checkFunction() #checks the parameters of occuring errors
                #additional task part
                try:
                    task = self.__tasks.get(block = True, timeout = 0.01) #looks up if there any tasks to do and terminates after 0.01 if None
                    try:
                        if len(task[1]) == 0:
                            self.__answs.put(task[0]()) #performs the task
                        else:
                            self.__answs.put(task[0](*task[1])) #performs the task
                    except Exception, err:
                        self.__answs.put('Could not execute the passed function because of: %s' % err) #gives back a error message
                    self.__writeProtocoll(task)
                except:
                    self.__writeProtocoll('')
        self.__protocollFile.close()
        
    def __perform(self, func, *args):
        '''manages the input and output from the Queues'''
        self.__tasks.put((func, args))
        return self.__answs.get()
        
    def __getILM(self):
        '''reads all important values of the ILM: [HeLVL, N2LVL]'''
        ret = []
        ret.append(self.ilm.getHeliumLevel())
        ret.append(self.ilm.getNitrogenLevel())
        return ret
    
    def __getITC(self):
        '''reads all important values of the ITC: [Temp1, Temp2, Temp3, SetpointT, ErrT, GasFlow, HeaterOutput]'''
        ret = []
        ret.append(self.itc.getTemperatur1())
        ret.append(self.itc.getTemperatur2())
        ret.append(self.itc.getTemperatur3())
        ret.append(self.itc.getSetpointTemperature())
        ret.append(self.itc.getTemperatureError())
        ret.append(self.itc.getGasflow())
        ret.append(self.itc.getHeaterOutput())
        return ret
    
    def __getLC(self):
        '''reads all important values of the LC: [Pressure, Temp, ValvePos]'''
        ret = []
        ret.append(self.lc.getPressure())
        ret.append(self.lc.getTemperature())
        ret.append(self.lc.getValvePosition())
        return ret
    
    def __getIPS(self):
        '''reads all important values of the IPS: [I_out, I_setpoint, I_magnet, I_pers, I_trip, B_out, B_setpoint, B_pers, B_trip, U_out, I_switchheater]'''
        ret = []
        ret.append(self.ips.getCurrentOutput())
        ret.append(self.ips.getCurrentSetpoint())
        ret.append(self.ips.getMagnetCurrent())
        ret.append(self.ips.getPersistentMagnetCurrent())
        ret.append(self.ips.getTripCurrent())
        ret.append(self.ips.getFieldOutput())
        ret.append(self.ips.getFieldSetpoint())
        ret.append(self.ips.getPersistentField())
        ret.append(self.ips.getTripField())
        ret.append(self.ips.getPowersupplyVoltage())
        ret.append(self.ips.getSwitchHeaterCurrent())
        return ret
    
    def __date(self):
        return dt.datetime.now().strftime('%Y-%m-%d') #year-month-day e.g. 2013-08-08
    
    def __time(self):
        return dt.datetime.now().strftime('%H-%M-%S') #hour-minute-second e.g. 13-18-10
    
    def __relTime(self, ref):
        return time.time() - ref
    
    def __writeProtocoll(self, *args):
        '''saves the data to protocoll file'''
        self.__protocollFile.write('%.2f\t' % self.__relTime(self.__startTime))
        for i in range(len(args)):
            self.__protocollFile.write('\t'.join(map(str, args[i]))+'\t')
        self.__protocollFile.write('\n')
        
    def __checkFunction(self):
        '''perform some safety checks...'''
        pass
    
    
    # tasks wich can be passed to the protocoll loop
    
    def __getSensors(self):
        '''returns the measured Parameter List'''
        return self.__ilmPars, self.__itcPars, self.__lcPars, self.__ipsPars
    
    def __setTemperature(self, temp):
        '''sets the system to desired temperature temp'''
        self.itc.setControlState('REMOTE+UNLOCKED')
        self.itc.setGasAUTO(0)
        self.itc.setHeaterAUTO(0)
        self.itc.setHeaterControlSensor(2)
        # set temp
        self.itc.setTemperature(temp)
        
        self.itc.setGasAUTO(1)
        self.itc.setHeaterAUTO(1)
        
        self.itc.setControlState('LOCAL+UNLOCKED')
            # (also including lambda fridge controll)
        return temp
    
    def __coupleMagnet(self):
        '''switches on the switch heater'''
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.holdValue()
        self.ips.turnOnSwitchHeater()
        self.ips.setControlState('LOCAL+UNLOCKED')
        return 'coupling'
        
    def __decoupleMagnet(self):
        '''switches off the switch heater'''
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.holdValue()
        self.ips.turnOffSwitchHeater()
        self.ips.setControlState('LOCAL+UNLOCKED')
        return 'decoupling'
    
    def __setCurrentMagneticField(self, field):
        '''sets the magnetic field to desired value field'''
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.setControlParameter('TESLA')
        self.ips.holdValue()
        self.ips.setField(field)
        self.ips.gotoSetpoint()
        self.ips.setControlState('LOCAL+UNLOCKED')
        return self.ips.getFieldSetpoint()
    
    def __gotoSourceZero(self):
        '''sets the current source value to zero'''
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.gotoZero()
        self.ips.setControlState('LOCAL+UNLOCKED')
        return 'goto zero'
    
    def __holdSourceValue(self):
        '''holds the current source value'''
        self.ips.setControlState('REMOTE+UNLOCKED')
        self.ips.holdValue()
        self.ips.setControlState('LOCAL+UNLOCKED')
        return 'hold source value'
    
    def __setToStandbyMode(self):
        '''sets the whole system into "not in use" mode'''
        self.itc.setControlState('REMOTE+UNLOCKED')
        # set initTemp, turn off AUTO-pid, heating+cooling
        self.itc.setGasAUTO(0)
        self.itc.setGasFlow(0)
        self.itc.setHeaterAUTO(0)
        self.itc.setHeaterOutput(0.0)
        
        self.itc.setControlState('LOCAL+UNLOCKED')
        self.ilm.setControlState('REMOTE+UNLOCKED')
        self.ilm.setHeliumProbeSpeed('SLOW')
        self.ilm.setControlState('LOCAL')
        # set initLambda fridge
    
    def __setToActiveMode(self):
        '''sets the whole system into "measurement" mode'''
        self.ilm.setControlState('REMOTE+UNLOCKED')
        self.ilm.setHeliumProbeSpeed('FAST')
        self.ilm.setControlState('LOCAL')
        # set levelmeter settings
        # set temperature controll settings
        # set magnet controll settings
        # set lambda fridge settings
        pass
    
    
    # public functions for controlling the cryostat
    
    def stop(self):
        '''stops the loop'''
        self.__stopRequest.set()
    
    def getSensors(self):
        '''returns the measurend parameter list'''
        ret = self.__perform(self.__getSensors)
        return ret
    
    def setTemperature(self, temp):
        '''sets the system to desired temperature temp'''
        __errors = []
        self.__perform(self.__setTemperature, temp) # gives the set temperature commands to ITC
        while 1:
            __vals = self.__perform(self.__getSensors) # measures temperature
            __errors.append(np.abs(__vals[1][4]))
            #if __vals[1][3] == temp:
            if np.sum(__errors[-20:])/float(len(__errors[-20:])) < 0.05: # continues if average error over las 20 values is below a limit
                break
        return __vals[1][0] # returns the last temperature value
    
    def setMagneticField(self, field):
        '''sets the magnetic field to desired value field'''
        __vals = self.__perform(self.__getSensors) # measures magnetic field
        __currField = __vals[3][5]
        __persField = __vals[3][7]
        if __persField == field:
            return __vals[3][5] # returns the last magnetic field value
        else:
            if __currField == __persField:
                self.__perform(self.__coupleMagnet)# couples the magnet
                print 'Wait 60 s for switch heater.'
                time.sleep(60) # waits till heater is ready
            else:
                self.__perform(self.__setCurrentMagneticField, __persField) # sets the src field to desired value (uncoupled in this case)
                __fields = []
                __errors = []
                while 1:
                    __vals = self.__perform(self.__getSensors) # measures magnetic field
                    __fields.append(__vals[3][5])
                    __errors.append(np.abs(__fields[-1] - __persField))
                    if np.sum(__errors[-5:])/float(len(__errors[-5:])) < 0.0001: # continues if average error over last 5 values is below a limit
                        break
                self.__perform(self.__coupleMagnet) # couples the magnet
                print 'Wait 60 s for switch heater.'
                time.sleep(60) # waits till heater is ready
            self.__perform(self.__setCurrentMagneticField, field) # sets the src field to desired value (coupled in this case)
            __fields = []
            __errors = []
            while 1:
                __vals = self.__perform(self.__getSensors) # measures magnetic field
                __fields.append(__vals[3][5])
                __errors.append(np.abs(__fields[-1] - field))
                if np.sum(__errors[-5:])/float(len(__errors[-5:])) < 0.0001: # continues if average error over last 5 values is below a limit
                    break
            self.__perform(self.__decoupleMagnet)
            print 'Wait 60 s for switch heater.'
            time.sleep(60) # waits till heater is ready
            
            self.__perform(self.__gotoSourceZero) # saves power because source goes to zero
            __fields = []
            __errors = []
            while 1:
                __vals = self.__perform(self.__getSensors) # measures magnetic field
                __fields.append(__vals[3][5])
                __errors.append(np.abs(__fields[-1]))
                if np.sum(__errors[-5:])/float(len(__errors[-5:])) < 0.0001: # continues if average error over last 5 values is below a limit
                    break
            
            return __vals[3][5] # returns the last magnetic field value
    
    def setToStandbyMode(self):
        '''sets the whole system into "not in use" mode'''
        ret = self.__perform(self.__setToStandbyMode)
        self.setMagneticField(0.0)
        return ret
    
    def setToActiveMode(self):
        '''sets the whole system into "measurement" mode'''
        ret = self.__perform(self.__setToActiveMode)
        return ret



# ######## #
# # Test # #
# ######## #

def test():
    #port = serial.Serial(port=0, baudrate=9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_TWO, timeout=10)
    #testILM(port)
    #testITC(port)
    #testLambdaController(port)
    #testIPS(port)
    testOxford()
    #port.close()
    
def testILM(port):
    ILM = ILM200(port)
    print 'Version: ', ILM.checkCommunication()
    print ILM.getState(decode = True)
    print 'I_cha1: ', ILM.getCha1Current()
    print 'I_cha2: ', ILM.getCha2Current()
    print 'Helium level: ', ILM.getHeliumLevel(), '%'
    print 'Nitrogen level: ', ILM.getNitrogenLevel(), '%'
    print 'Valve position: ', ILM.getValvePosition()
    ILM.setControlState('REMOTE+LOCKED')
    ILM.setHeliumProbeSpeed('FAST')
    #print ILM.getState(decode = False)
    ILM.setHeliumProbeSpeed('SLOW')
    ILM.setControlState('REMOTE+UNLOCKED')
    ILM.setControlState('LOCAL')
    
def testITC(port):
    ITC = ITC503(port)
    print 'Version: ', ITC.checkCommunication()
    print ITC.getState(decode = True)
    print 'Temp1: ', ITC.getTemperatur1(), 'K'
    print 'Temp2: ', ITC.getTemperatur2(), 'K'
    print 'Temp3: ', ITC.getTemperatur3(), 'K'
    print 'Temp error: ', ITC.getTemperatureError(), 'K'
    initT = ITC.getSetpointTemperature()
    #ITC.setControlState('REMOTE+LOCKED')
    ITC.setControlState('REMOTE+UNLOCKED')
    ITC.setGasAUTO(0)
    ITC.setHeaterAUTO(0)
    ITC.setTemperature(100.0)
    #time.sleep(10)
    print 'PID: P(%f), I(%f), D(%f)' % (ITC.getP(), ITC.getI(), ITC.getD())
    print 'Setpoint: ', ITC.getSetpointTemperature(), 'K'
    ITC.setGasFlow(50.0)
    print 'Gas flow: ', ITC.getGasflow(), '%'
    time.sleep(5)
    print 'Gas flow: ', ITC.getGasflow(), '%'
    ITC.setHeaterControlSensor(2)
    time.sleep(5)
    ITC.setHeaterControlSensor(3)
    time.sleep(5)
    ITC.setHeaterControlSensor(1)
    ITC.setMaximumHeaterLimit(20.0)
    ITC.setHeaterOutput(10.0)
    time.sleep(5)
    print 'Heater output: ', ITC.getHeaterOutput(volt = True), 'V'
    print 'Heater output: ', ITC.getHeaterOutput(volt = False), '%'
    ITC.setHeaterOutput(0.0)
    ITC.setTemperature(initT)
    #ITC.setGasAUTO(0)
    #ITC.setHeaterAUTO(0)
    ITC.turnOnAutoPID()
    time.sleep(1)
    print 'PID: P(%f), I(%f), D(%f)' % (ITC.getP(), ITC.getI(), ITC.getD())
    print 'Setpoint: ', ITC.getSetpointTemperature(), 'K'
    time.sleep(4)
    ITC.turnOffAutoPID()
    ITC.setControlState('LOCAL+UNLOCKED')
    #map(ITC.setTemperature, [1.55, 3.0, 4.2, 5.0, 6.0, 10.0, 15.0, 20.0, 50.0, 75.0, 100.0, 150.0, 200.0, 250.0, 300.0])
    
def testLambdaController(port):
    LC = LambdaController(port)
    print 'Version: ', LC.checkCommunication()
    print LC.getState(decode = True)
    print 'Pressure: ', LC.getPressure(), 'mbar'
    print 'Valve position: ', LC.getValvePosition()
    print 'Temperature: ', LC.getTemperature(), 'K'
    LC.setControlState('ACTIVE')
    LC.setGasFlow(99)
    time.sleep(5)
    LC.setGasFlow(0)
    LC.setControlState('STANDBY')
    
def testIPS(port):
    IPS = IPS120(port)
    print 'Version: ', IPS.checkCommunication()
    print IPS.getState(decode = True)
    print 'I_out: ', IPS.getCurrentOutput(), 'A'
    print 'I_setpoint: ', IPS.getCurrentSetpoint(), 'A'
    print 'I_sweeprate: ', IPS.getCurrentSweepRate(), 'A/min'
    print 'I_magnet: ', IPS.getMagnetCurrent(), 'A'
    print 'I_magnet-pers: ', IPS.getPersistentMagnetCurrent(), 'A'
    print 'I_trip: ', IPS.getTripCurrent(), 'A'
    print 'I_neg-lim: ', IPS.getNegativeCurrentLimit(), 'A'
    print 'I_pos-lim: ', IPS.getPositiveCurrentLimit(), 'A'
    print 'B_out: ', IPS.getFieldOutput(), 'T'
    print 'B_setpoint: ', IPS.getFieldSetpoint(), 'T'
    print 'B_sweeprate: ', IPS.getFieldSweepRate(), 'T/min'
    print 'B_pers: ', IPS.getPersistentField(), 'T'
    print 'B_trip: ', IPS.getTripField(), 'T'
    print 'Inductance: ', IPS.getMagnetInductance(), 'H'
    print 'Lead resistance: ', IPS.getLeadResistance(), 'mOhm'
    print 'U_out: ', IPS.getPowersupplyVoltage(), 'V'
    print 'U_limit: ', IPS.getVoltageLimit(), 'V'
    print 'I_switchheater: ', IPS.getSwitchHeaterCurrent(), 'mA'
    IPS.setControlState('REMOTE+UNLOCKED')
    time.sleep(5)
    IPS.setControlState('LOCAL+LOCKED')
    
    
def testOxford():
    Cryo = OxfordControl()
    Cryo.start()
    #time.sleep(10)
    #print Cryo.getSensors()
    #Cryo.setTemperature(153.0)
    #time.sleep(5)
    #print Cryo.setTemperature(155.0)
    #Cryo.setMagneticField(0.1)
    #time.sleep(10)
    #Cryo.setMagneticField(0.2)
    #time.sleep(10)
    #Cryo.setMagneticField(0.3)
    #time.sleep(10)
    #Cryo.setMagneticField(0.4)
    #time.sleep(10)
    #Cryo.setMagneticField(0.5)
    #time.sleep(10)
    #Cryo.setToStandbyMode()
    #time.sleep(10)
    print Cryo.getSensors()
    Cryo.stop()

if __name__ == '__main__':
    test()
