import express, { type Request, type Response } from "express";
import http from "http";
import { WebSocketServer } from "ws";
import WebSocket from "ws";
import cors from "cors";
import "dotenv/config";

const PORT = process.env.PORT;

const app = express();
app.use(cors());
app.use(express.json());

const server = http.createServer(app);
const wss = new WebSocketServer({ server });

let clients: WebSocket[] = [];

wss.on("connection", (socket) => {
  console.log("Client connected!");

  clients.push(socket);

  socket.on("close", () => {
    clients = clients.filter((client) => client !== socket);
    console.log("Client disconnected!");
  });
});

app.post("/send-data", (req: Request, res: Response) => {
  const data = req.body;

  console.log(`Received data from FastAPI: ${data}`);

  clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify(data));
    }
  });

  res.status(200).json({ message: "Send to clients" });
});

server.listen(PORT, () => {
  console.log(`Server is listening on ${PORT}`);
});
