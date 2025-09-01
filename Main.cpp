#include <QTimer>

#include "Trace.h"
#include "ds18b20.h"
#include "Thermostats.h"
#include "Relay.h"
#include "mainwindow.h"
#include "consignes.h"

//#include "filtre.h"

//Filtre *m_initFiltre;

class Cooling {

public:
    void disable() {enabled = false;}
    void enable() {enabled = true;}
    void display_status() {
        if (enabled) {TRACE("Froid: ENABLED\n");}
        else {TRACE("Froid: DISABLED\n");}
    }

private:
    bool enabled {false};
};

class Heating {

public:
    void disable() {enabled = false;}
    void enable() {enabled = true;}
    void display_status() {
	if (enabled) {TRACE("Chauffage: ENABLED\n");}
	else {TRACE("Chauffage: DISABLED\n");}
    }
	
private:
    bool enabled {false};
};

class Automatic {

public:
    void disable() {enabled = false;}
    void enable() {enabled = true;}
    void display_status() {
	if (enabled) {TRACE("Automatique: ENABLED\n");}
	else {TRACE("Automatique: DISABLED\n");}
    }

private:
    bool enabled {false};
};
 
enum Mode {OFF, HEATING, COOLING, AUTO, MAINTENANCE};
 
class PAC { // Pompe_A_Chaleur

public:    
    void run_pac() {
	if (read_thermostat_room()) {
	    if (read_sensorOutside() <= 13.5) {
		TRACE("Setting chauffage mode...\n");
		set_mode(HEATING);
	    } else {
		TRACE("Setting froid mode...\n");
		set_mode(COOLING);
	    }
	} else {
	    TRACE("Setting off mode ...\n");
	    set_mode(OFF);
	}
    }

private:
    Heating chaud {};
    Cooling froid {};
    Automatic automatique {};
    DS18B20 sensorOutside {};
    Thermostat thermostatRoom {};
    Relay orderRelay {};
    
    Mode mode {OFF};
    
    void set_mode(Mode m) {
		
        mode = m;
        
        switch (m) {
			
	    case OFF:
	    TRACE("OFF\n");
	    
	    disable_all();
	    orderRelay.lowAllRelay();
			
	    break;
			
	    case COOLING:
	    TRACE("COOLING\n");
		    
	    enable_froid();
	    orderRelay.lowRelayEteHiver();
			
	    break;
			
	    case HEATING:
	    TRACE("HEATING\n");
			
	    enable_chaud();
	    orderRelay.highRelayEteHiver();
			
	    break;
			
	    case AUTO:
			
	    enable_auto();
			
	    break;
			
	    case MAINTENANCE:
			
	    //enable_maintenance();
			
	    break;
        }
        display_status();
    }
    
    Mode get_mode() {return mode;} // en réponse à la question précédente, si besoin d'un get_mode()
    
    double read_sensorOutside() {return sensorOutside.get_tempExt();}
    
    void display_outdoor_temperature() {TRACE("sensor outside : %2.2f\n", sensorOutside.get_tempExt());}
    
    bool read_thermostat_room() {TRACE("thermostat is: %s", thermostatRoom.get_etatThermostat() ? "true\n" : "false\n"); return thermostatRoom.get_etatThermostat();}

    void enable_chaud() {automatique.disable(); froid.disable(); chaud.enable();} //orderRelay.highRelayEteHiver();}
    void enable_froid() {automatique.disable(); chaud.disable(); froid.enable();} //orderRelay.lowRelayEteHiver();}
    void enable_auto() {froid.disable(); chaud.disable(); automatique.enable();}
    
    void disable_all() {froid.disable(); chaud.disable(); automatique.disable();} //orderRelay.lowAllRelay();}
    
    void display_status() {display_outdoor_temperature(); froid.display_status(); chaud.display_status(); automatique.display_status();}
};

void read_mode_Pac()
{
	static PAC pac;
    pac.run_pac();
}

int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor)); // cache pointeur Sourie
    
    MainWindow window;
    
    Consignes *initConsignes;
    
    initConsignes = new Consignes(QString("settings/consignes.ini"), QSettings::IniFormat);
    initConsignes ->controleConsignes();
    
    QTimer timerPac;
    int timePac {1000};
    QObject::connect(&timerPac, &QTimer::timeout, read_mode_Pac);
    timerPac.start(timePac);
    
    return app.exec();
}
