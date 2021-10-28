#include "config.h"
#include "BMSModuleManager.h"

BMSModuleManager::BMSModuleManager()
{
  for (int i = 1; i <= MAX_MODULE_ADDR; i++) {
    modules[i].setExists(false);
    modules[i].setAddress(i);
  }
  lowestPackVolt = 1000.0f;
  highestPackVolt = 0.0f;
  lowestPackTemp = 200.0f;
  highestPackTemp = -100.0f;
  isFaulted = false;
}

bool BMSModuleManager::checkcomms()
{
  int g = 0;
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      g = 1;
      if (modules[y].isReset())
      {
        //Do nothing as the counter has been reset
      }
      else
      {
        modules[y].setExists(false);
        return false;
      }
    }
    modules[y].setReset(false);
    modules[y].setAddress(y);
  }
  if ( g == 0)
  {
    return false;
  }
  return true;
}

int BMSModuleManager::seriescells()
{
  spack = 0;
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      spack = spack + modules[y].getscells();
    }
  }
  return spack;
}

void BMSModuleManager::clearmodules()
{
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      modules[y].clearmodule();
      modules[y].setExists(false);
      modules[y].setAddress(y);
    }
  }
}

void BMSModuleManager::decodetemp(BMS_CAN_MESSAGE &msg, int debug)
{
  int CMU = (msg.id & 0xFF);
  if (debug == 1)
  {
    Serial.println();
    Serial.print(CMU);
    Serial.print(" | ");
  }
  
  // The code below i belive should be deleted as it prevents tempertures from being read rom the second canbus (temp can id's offset by 8), so they would go from 9 to 16 and you wouldnt see modules 11+  
  /*
  if (CMU > 10 && CMU < 60)
  {
    CMU = (CMU * 0.5) - 15;
  }
  /*
    if (CMU > 64 && CMU < 73)
    {
    CMU = CMU - 48;
    }
    if (CMU > 97 && CMU < 104)
    {
    CMU = CMU - 72;
    }
  
  if (CMU > 0 && CMU < 15);
  {
    modules[CMU].setExists(true);
    modules[CMU].setReset(true);
  */
    modules[CMU].decodetemp(msg);
    if (debug == 1)
    {
      Serial.println();
      Serial.print(CMU);
      Serial.print(" Temp Found");

    }
  //}
}

void BMSModuleManager::decodecan(BMS_CAN_MESSAGE &msg, int debug)
{
  int CMU, Id = 0;
  switch (msg.id)
  {
    /*
      ///////////////// Two extender increment//////////
      case (0x210):
      CMU = 25;
      Id = 0;
      break;
      case (0x211):
      CMU = 25;
      Id = 1;
      break;
      case (0x212):
      CMU = 25;
      Id = 2;
      break;

      case (0x214):
      CMU = 26;
      Id = 0;
      break;
      case (0x215):
      CMU = 26;
      Id = 1;
      break;
      case (0x216):
      CMU = 26;
      Id = 2;
      break;
      case (0x218):
      CMU = 27;
      Id = 0;
      break;
      case (0x219):
      CMU = 27;
      Id = 1;
      break;
      case (0x21A):
      CMU = 27;
      Id = 2;
      break;
      case (0x21C):
      CMU = 28;
      Id = 0;
      break;
      case (0x21D):
      CMU = 28;
      Id = 1;
      break;
      case (0x21E):
      CMU = 28;
      Id = 2;
      break;

      case (0x220):
      CMU = 29;
      Id = 0;
      break;
      case (0x221):
      CMU = 29;
      Id = 1;
      break;
      case (0x222):
      CMU = 29;
      Id = 2;
      break;

      case (0x224):
      CMU = 30;
      Id = 0;
      break;
      case (0x225):
      CMU = 30;
      Id = 1;
      break;
      case (0x226):
      CMU = 30;
      Id = 2;
      break;

      case (0x228):
      CMU = 31;
      Id = 0;
      break;
      case (0x229):
      CMU = 31;
      Id = 1;
      break;
      case (0x22A):
      CMU = 31;
      Id = 2;
      break;

      case (0x22C):
      CMU = 32;
      Id = 0;
      break;
      case (0x22D):
      CMU = 32;
      Id = 1;
      break;
      case (0x22E):
      CMU = 32;
      Id = 2;
      break;


      ///////////////// Two extender increment//////////
      case (0x1F0):
      CMU = 17;
      Id = 0;
      break;
      case (0x1F1):
      CMU = 17;
      Id = 1;
      break;
      case (0x1F2):
      CMU = 17;
      Id = 2;
      break;

      case (0x1F4):
      CMU = 18;
      Id = 0;
      break;
      case (0x1F5):
      CMU = 18;
      Id = 1;
      break;
      case (0x1F6):
      CMU = 18;
      Id = 2;
      break;

      case (0x1F8):
      CMU = 19;
      Id = 0;
      break;
      case (0x1F9):
      CMU = 19;
      Id = 1;
      break;
      case (0x1FA):
      CMU = 19;
      Id = 2;
      break;

      case (0x1FC):
      CMU = 20;
      Id = 0;
      break;
      case (0x1FD):
      CMU = 20;
      Id = 1;
      break;
      case (0x1FE):
      CMU = 20;
      Id = 2;
      break;

      case (0x200):
      CMU = 21;
      Id = 0;
      break;
      case (0x201):
      CMU = 21;
      Id = 1;
      break;
      case (0x202):
      CMU = 21;
      Id = 2;
      break;

      case (0x204):
      CMU = 22;
      Id = 0;
      break;
      case (0x205):
      CMU = 22;
      Id = 1;
      break;
      case (0x206):
      CMU = 22;
      Id = 2;
      break;

      case (0x208):
      CMU = 23;
      Id = 0;
      break;
      case (0x209):
      CMU = 23;
      Id = 1;
      break;
      case (0x20A):
      CMU = 23;
      Id = 2;
      break;

      case (0x20C):
      CMU = 24;
      Id = 0;
      break;
      case (0x20D):
      CMU = 24;
      Id = 1;
      break;
      case (0x20E):
      CMU = 24;
      Id = 2;
      break;
    */
    ///////////////// one extender increment//////////

    case (0x1D0):
      CMU = 9;
      Id = 0;
      break;
    case (0x1D1):
      CMU = 9;
      Id = 1;
      break;
    case (0x1D2):
      CMU = 9;
      Id = 2;
      break;
    case (0x1D3):
      CMU = 9;
      Id = 3;
      break;

    case (0x1D4):
      CMU = 10;
      Id = 0;
      break;
    case (0x1D5):
      CMU = 10;
      Id = 1;
      break;
    case (0x1D6):
      CMU = 10;
      Id = 2;
      break;
    case (0x1D8):
      CMU = 11;
      Id = 0;
      break;
    case (0x1D9):
      CMU = 11;
      Id = 1;
      break;
    case (0x1DA):
      CMU = 11;
      Id = 2;
      break;
    case (0x1DC):
      CMU = 12;
      Id = 0;
      break;
    case (0x1DD):
      CMU = 12;
      Id = 1;
      break;
    case (0x1DE):
      CMU = 12;
      Id = 2;
      break;

    case (0x1E0):
      CMU = 13;
      Id = 0;
      break;
    case (0x1E1):
      CMU = 13;
      Id = 1;
      break;
    case (0x1E2):
      CMU = 13;
      Id = 2;
      break;

    case (0x1E4):
      CMU = 14;
      Id = 0;
      break;
    case (0x1E5):
      CMU = 14;
      Id = 1;
      break;
    case (0x1E6):
      CMU = 14;
      Id = 2;
      break;

    case (0x1E8):
      CMU = 15;
      Id = 0;
      break;
    case (0x1E9):
      CMU = 15;
      Id = 1;
      break;
    case (0x1EA):
      CMU = 15;
      Id = 2;
      break;

    case (0x1EC):
      CMU = 16;
      Id = 0;
      break;
    case (0x1ED):
      CMU = 16;
      Id = 1;
      break;
    case (0x1EE):
      CMU = 16;
      Id = 2;
      break;


    ///////////////////////standard ids////////////////


    case (0x1B0):
      CMU = 1;
      Id = 0;
      break;
    case (0x1B1):
      CMU = 1;
      Id = 1;
      break;
    case (0x1B2):
      CMU = 1;
      Id = 2;
      break;
    case (0x1B3):
      CMU = 1;
      Id = 3;
      break;

    case (0x1B4):
      CMU = 2;
      Id = 0;
      break;
    case (0x1B5):
      CMU = 2;
      Id = 1;
      break;
    case (0x1B6):
      CMU = 2;
      Id = 2;
      break;
    case (0x1B7):
      CMU = 2;
      Id = 3;
      break;

    case (0x1B8):
      CMU = 3;
      Id = 0;
      break;
    case (0x1B9):
      CMU = 3;
      Id = 1;
      break;
    case (0x1BA):
      CMU = 3;
      Id = 2;
      break;
    case (0x1BB):
      CMU = 3;
      Id = 3;
      break;

    case (0x1BC):
      CMU = 4;
      Id = 0;
      break;
    case (0x1BD):
      CMU = 4;
      Id = 1;
      break;
    case (0x1BE):
      CMU = 4;
      Id = 2;
      break;
    case (0x1BF):
      CMU = 4;
      Id = 3;
      break;

    case (0x1C0):
      CMU = 5;
      Id = 0;
      break;
    case (0x1C1):
      CMU = 5;
      Id = 1;
      break;
    case (0x1C2):
      CMU = 5;
      Id = 2;
      break;
    case (0x1C3):
      CMU = 5;
      Id = 3;
      break;

    case (0x1C4):
      CMU = 6;
      Id = 0;
      break;
    case (0x1C5):
      CMU = 6;
      Id = 1;
      break;
    case (0x1C6):
      CMU = 6;
      Id = 2;
      break;
    case (0x1C7):
      CMU = 6;
      Id = 3;
      break;

    case (0x1C8):
      CMU = 7;
      Id = 0;
      break;
    case (0x1C9):
      CMU = 7;
      Id = 1;
      break;
    case (0x1CA):
      CMU = 7;
      Id = 2;
      break;
    case (0x1CB):
      CMU = 7;
      Id = 3;
      break;

    case (0x1CC):
      CMU = 8;
      Id = 0;
      break;
    case (0x1CD):
      CMU = 8;
      Id = 1;
      break;
    case (0x1CE):
      CMU = 8;
      Id = 2;
      break;
    case (0x1CF):
      CMU = 8;
      Id = 3;
      break;

    default:
      return;
      break;
  }
  if (CMU > 0 && CMU < 64)
  {
    if (debug == 1)
    {
      Serial.println();
      Serial.print(CMU);
      Serial.print(",");
      Serial.print(Id);
      Serial.println();
    }
    modules[CMU].setExists(true);
    modules[CMU].setReset(true);
    modules[CMU].decodecan(Id, msg);
  }
}


void BMSModuleManager::getAllVoltTemp()
{
  packVolt = 0.0f;

  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      packVolt += modules[x].getModuleVoltage();
      if (modules[x].getLowTemp() < lowestPackTemp) lowestPackTemp = modules[x].getLowTemp();
      if (modules[x].getHighTemp() > highestPackTemp) highestPackTemp = modules[x].getHighTemp();
    }
  }

  packVolt = packVolt / Pstring;
  if (packVolt > highestPackVolt) highestPackVolt = packVolt;
  if (packVolt < lowestPackVolt) lowestPackVolt = packVolt;

//TODO::What is this?
  if (digitalRead(11) == LOW) {
    isFaulted = true;
  }
  else
  {
    isFaulted = false;
  }
}

float BMSModuleManager::getLowCellVolt()
{
  LowCellVolt = 5.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getLowCellV() <  LowCellVolt)  LowCellVolt = modules[x].getLowCellV();
    }
  }
  return LowCellVolt;
}

float BMSModuleManager::getHighCellVolt()
{
  HighCellVolt = 0.0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      if (modules[x].getHighCellV() >  HighCellVolt)  HighCellVolt = modules[x].getHighCellV();
    }
  }
  return HighCellVolt;
}

float BMSModuleManager::getPackVoltage()
{
  return packVolt;
}

int BMSModuleManager::getNumModules()
{
  return numFoundModules;
}

float BMSModuleManager::getLowVoltage()
{
  return lowestPackVolt;
}

float BMSModuleManager::getHighVoltage()
{
  return highestPackVolt;
}

void BMSModuleManager::setBatteryID(int id)
{
  batteryID = id;
}

void BMSModuleManager::setPstrings(int Pstrings)
{
  Pstring = Pstrings;
}

void BMSModuleManager::setSensors(int sensor, float Ignore, float VoltDelta)
{
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      modules[x].settempsensor(sensor);
      modules[x].setIgnoreCell(Ignore);
      modules[x].setDelta(VoltDelta);
    }
  }
}

float BMSModuleManager::getAvgTemperature()
{
  float avg = 0.0f;
  lowTemp = 999.0f;
  highTemp = -999.0f;
  int y = 0; //counter for modules below -70 (no sensors connected)
  numFoundModules = 0;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting())
    {
      numFoundModules++;
      if (modules[x].getAvgTemp() > -70)
      {
        avg += modules[x].getAvgTemp();
        if (modules[x].getHighTemp() > highTemp)
        {
          highTemp = modules[x].getHighTemp();
        }
        if (modules[x].getLowTemp() < lowTemp)
        {
          lowTemp = modules[x].getLowTemp();
        }
      }
      else
      {
        y++;
      }
    }
  }
  avg = avg / (float)(numFoundModules - y);

  return avg;
}

float BMSModuleManager::getHighTemperature()
{
  return highTemp;
}

float BMSModuleManager::getLowTemperature()
{
  return lowTemp;
}

float BMSModuleManager::getAvgCellVolt()
{
  float avg = 0.0f;
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
  {
    if (modules[x].isExisting()) avg += modules[x].getAverageV();
  }
  avg = avg / (float)numFoundModules;

  return avg;
}

void BMSModuleManager::printPackSummary()
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;

  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();


      if (faults > 0)
      {
        if (faults & 1)
        {
          Serial.print("    Overvoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 12; i++)
          {
            if (COV & (1 << i))
            {
              Serial.print(i + 1);
              Serial.print(" ");
            }
          }
          Serial.println();
        }
        if (faults & 2)
        {
          Serial.print("    Undervoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 12; i++)
          {
            if (CUV & (1 << i))
            {
              Serial.print(i + 1);
              Serial.print(" ");
            }
          }
          Serial.println();
        }
      }
    }
  }
}

void BMSModuleManager::printPackDetailsJson(DynamicJsonDocument &root)
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;
  int cellNum = 0;

  JsonArray modulesNode = root.createNestedArray("modules");
  for (int y = 1; y < 63; y++)
  {

    if (modules[y].isExisting())
    {
      JsonObject moduleNode = modulesNode.createNestedObject();
      moduleNode["module"] = y;
      moduleNode["total_voltage"] = modules[y].getModuleVoltage();
      moduleNode["lowest_voltage"] =  modules[y].getLowCellV();
      moduleNode["highest_voltage"] =  modules[y].getHighCellV();
      moduleNode["average_voltage"] =  modules[y].getAverageV();
      moduleNode["temperature1"] =  modules[y].getTemperature(0);
      moduleNode["temperature2"] =  modules[y].getTemperature(1);
      moduleNode["temperature3"] =  modules[y].getTemperature(2);
      JsonArray cellsNode = moduleNode.createNestedArray("cells");
      if (y > 1) {
        Serial.print(",");
      }
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();
      
      for (int i = 0; i < 13; i++)
      {
        JsonObject cellNode = cellsNode.createNestedObject();
        cellNode["cell"] = cellNum++;
        cellNode["voltage"] = modules[y].getCellVoltage(i);
      }
    }
  }

}

void BMSModuleManager::printPackDetails(int digits)
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;
  int cellNum = 0;

  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();

      Serial.print("Module #");
      Serial.print(y);
      if (y < 10) Serial.print(" ");
      Serial.print("  ");
      Serial.print(modules[y].getModuleVoltage(), digits);
      Serial.print("V");
      for (int i = 0; i < 13; i++)
      {
        if (cellNum < 10) Serial.print(" ");
        Serial.print("  Cell");
        Serial.print(cellNum++);
        Serial.print(": ");
        Serial.print(modules[y].getCellVoltage(i), digits);
        Serial.print("V");
      }
      Serial.println();
      Serial.print(" Temp 1: ");
      Serial.print(modules[y].getTemperature(0));
      Serial.print("C Temp 2: ");
      Serial.print(modules[y].getTemperature(1));
      Serial.print("C Temp 3: ");
      Serial.print(modules[y].getTemperature(2));
      Serial.println("C");
    }
  }
}
void BMSModuleManager::printAllCSV(unsigned long timestamp, float current, int SOC)
{
  for (int y = 1; y < 63; y++)
  {
    if (modules[y].isExisting())
    {
      Serial.print(timestamp);
      Serial.print(",");
      Serial.print(current, 0);
      Serial.print(",");
      Serial.print(SOC);
      Serial.print(",");
      Serial.print(y);
      Serial.print(",");
      for (int i = 0; i < 8; i++)
      {
        Serial.print(modules[y].getCellVoltage(i));
        Serial.print(",");
      }
      Serial.print(modules[y].getTemperature(0));
      Serial.print(",");
      Serial.print(modules[y].getTemperature(1));
      Serial.print(",");
      Serial.print(modules[y].getTemperature(2));
      Serial.println();
    }
  }
  
}