import { useEffect, useState } from "react";
import { WS_URL } from "../config/config.index";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
} from "recharts";

interface SensorData {
  time: string;
  aqi: number;
  temp: number;
  humidity: number;
}

const MAX_POINTS = 30;

export default function Dashboard() {
  const [data, setData] = useState<SensorData[]>([]);
  const [aqi, setAqi] = useState<string>("0");
  const [temp, setTemp] = useState<string>("0");
  const [humidity, setHumidity] = useState<string>("0");
  const [airClass, setAirClass] = useState<string>("0");
  const [message, setMessage] = useState<string>("0");

  useEffect(() => {
    const ws = new WebSocket(WS_URL);

    ws.onmessage = (e) => {
      const parsedData = JSON.parse(e.data);

      const newPoint: SensorData = {
        time: new Date().toLocaleTimeString([], {
          minute: "2-digit",
          second: "2-digit",
        }),
        aqi: Number(parsedData.aqi),
        temp: Number(parsedData.temperature),
        humidity: Number(parsedData.humidity),
      };

      setData((prev) => {
        const updated = [...prev, newPoint];
        return updated.slice(-MAX_POINTS); // memory safe
      });

      setAqi(parsedData.aqi);
      setTemp(parsedData.temperature);
      setHumidity(parsedData.humidity);
      setAirClass(parsedData.airClass);
      setMessage(parsedData.message);
    };

    return () => {
      ws.close();
    };
  }, []);

  return (
    <div className="flex flex-col items-center gap-5 tracking-tight bg-[#f77f7]">
      {/* Header */}
      <div className="w-full flex justify-center items-center px-10 py-6">
        <div className="flex gap-1 text-4xl font-semibold tracking-tighter">
          <h2>Air</h2>
          <h2 className="text-orange-500">Q</h2>
        </div>

        {/* <p className="bg-black text-gray-200 font-medium px-5 py-1 rounded-xl">
          Github
        </p> */}
      </div>

      {/* Divs */}
      <div className="flex items-center gap-9">
        <div className="flex flex-col gap-15 border border-gray-200 px-8 py-8 rounded-xl min-w-75 shadow-sm bg-white">
          <div className="flex justify-between items-center">
            <h2 className="text-xl font-medium">Air Quality</h2>

            <p className="border border-gray-300 bg-blue-50 px-3 py-1 rounded-lg tracking-normal font-medium">
              {airClass}
            </p>
          </div>

          <p className="text-3xl text-red-500">{aqi.toString().slice(0, -2)}</p>
        </div>

        <div className="flex flex-col gap-15 border border-gray-200 px-8 py-8 rounded-xl min-w-75 shadow-sm bg-white">
          <h2 className="text-xl font-medium">Temperature</h2>

          <p className="text-3xl text-green-500">{temp} °C</p>
        </div>

        <div className="flex flex-col gap-15 border border-gray-200 px-8 py-8 rounded-xl min-w-75 shadow-sm bg-white">
          <h2 className="text-xl font-medium">Humidity</h2>

          <p className="text-3xl text-blue-500">{humidity} %</p>
        </div>

        <div className="flex flex-col gap-10 border border-gray-200 px-8 py-8 rounded-xl max-w-75 shadow-sm bg-white">
          <h2 className="text-xl font-medium">Feedback</h2>

          <p className="text-xl text-sky-500">{message}</p>
        </div>
      </div>

      <div className="flex mt-4 mr-15 gap-5">
        {/* AQI Graph */}
        <div>
          <h3 className="text-lg font-semibold text-center">AQI</h3>
          <LineChart width={440} height={350} data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="time" />
            <YAxis />
            <Tooltip />
            <Line type="monotone" dataKey="aqi" stroke="#8884d8" />
          </LineChart>
        </div>

        {/* Temperature Graph */}
        <div>
          <h3 className="text-lg font-semibold text-center">
            Temperature (°C)
          </h3>
          <LineChart width={440} height={350} data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="time" />
            <YAxis />
            <Tooltip />
            <Line type="monotone" dataKey="temp" stroke="#82ca9d" />
          </LineChart>
        </div>

        {/* Humidity Graph */}
        <div className="flex justify-center">
          <div>
            <h3 className="text-lg font-semibold text-center">Humidity (%)</h3>
            <LineChart width={440} height={350} data={data}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis dataKey="time" />
              <YAxis />
              <Tooltip />
              <Line type="monotone" dataKey="humidity" stroke="#ff7300" />
            </LineChart>
          </div>
        </div>
      </div>
    </div>
  );
}
