import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class ApiService {
  private baseUrl = 'http://10.3.2.124:5000';

  constructor(private http: HttpClient) {}

  // **Device Methods**
  getDevices(): Observable<any[]> {
    return this.http.get<any[]>(`${this.baseUrl}/end_devices`);
  }

  getDeviceById(id: number): Observable<any> {
    return this.http.get<any>(`${this.baseUrl}/end_devices/${id}`);
  }

  getDeviceByName(name: string): Observable<any> {
    return this.http.get<any>(`${this.baseUrl}/end_devices/name/${name}`);
  }

  updateDevice(name: string, device:any): Observable<any> {
    return this.http.put<any>(`${this.baseUrl}/end_devices/modify/${name}`, device);
  }

  addDevice(device: any): Observable<any> {
    return this.http.post<any>(`${this.baseUrl}/end_devices`, device);
  }

  deleteDevice(id: number): Observable<any> {
    return this.http.delete<any>(`${this.baseUrl}/end_devices/${id}`);
  }

  getWaveInfoByDeviceId(id: number): Observable<any> {
    return this.http.get<any>(`${this.baseUrl}/wave_infos/${id}`);
  }

  getWaveInfosFromRange(start_time: Date, end_time: Date): Observable<any> {
    const start = start_time.toISOString();
    const end = end_time.toISOString();

    return this.http.post<any>(`${this.baseUrl}/wave_infos/timerange`, {
      start_time: start,
      end_time: end,
    });
  }

  getWeatherInfoByDeviceId(id: number): Observable<any> {
    return this.http.get<any>(`${this.baseUrl}/weather_station/${id}`);
  }

  getWeatherInfosFromRange(start_time: Date, end_time: Date): Observable<any> {
    const start = start_time.toISOString();
    const end = end_time.toISOString();

    return this.http.post<any>(`${this.baseUrl}/weather_station/timerange`, {
      start_time: start,
      end_time: end,
    });
  }

  findDevice(device: string, executionTime: number, target: number): Observable<any> {
    return this.http.post<any>(
      `${this.baseUrl}/find_device`, 
      { device, executionTime, target },
      { headers: { 'Content-Type': 'application/json' } }
    );
  }
  
}
