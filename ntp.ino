
const int daylightOffset_sec = 3600;
const char* ntpServer = "nz.pool.ntp.org";

void startNTP()
{
  configTime(userUTCOffset, daylightOffset_sec, ntpServer);
}

String getISO8601Time(boolean zeroSeconds)
{
  if (apMode && !rtcManuallySet)
  {
    return String("0000-01-01T00:00:00");
  }
  ESP32Time rtc;
  if (zeroSeconds)
  {
    return rtc.getTime("%Y-%m-%dT%H:%M:00");
  }
  else
  {
    return rtc.getTime("%Y-%m-%dT%H:%M:%S");
  }
}

void setRTCTime(String newTime)
{
  ESP32Time rtc;
  int y, M, d, h, m;
  float s;
  sscanf(newTime.c_str(), "%d-%d-%dT%d:%d:%d", &y, &M, &d, &h, &m, &s);
  rtc.setTime(s, m, h, d, M, y);
  rtcManuallySet = true;
}
