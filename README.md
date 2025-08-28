

# 🚀 Gemini Chatbot (C++ + Crow)

This project is a lightweight **AI chatbot backend** written in **C++** using [Crow](https://github.com/CrowCpp/crow) (a C++ web framework), [cURL](https://curl.se/libcurl/), and [nlohmann/json](https://github.com/nlohmann/json).
It connects to **Google Gemini API** and provides a simple REST API for chatting with an AI assistant.

---

## ✨ Features

* ✅ REST API built with **Crow**
* ✅ Persistent **multi-turn conversation history** per session
* ✅ Uses **Google Gemini API** (`gemini-1.5-flash`) for AI responses
* ✅ Thread-safe session management with `std::mutex`
* ✅ Simple frontend support (serves `index.html`)

---

## 🛠️ Requirements

* **C++17 or later**
* [Crow](https://github.com/CrowCpp/crow)
* [libcurl](https://curl.se/libcurl/)
* [nlohmann/json](https://github.com/nlohmann/json)

### Install on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install g++ cmake libcurl4-openssl-dev
```

### Install on Windows (via vcpkg)

```bash
vcpkg install curl nlohmann-json
```

---

## ⚡ Setup & Run

### 1. Clone the repo

```bash
git clone https://github.com/your-username/gemini-cpp-chatbot.git
cd gemini-cpp-chatbot
```

### 2. Set your Gemini API Key

Edit `main.cpp` and replace:

```cpp
const string API_KEY = "YOUR_API_KEY_HERE";
```

You can get an API key from [Google AI Studio](https://aistudio.google.com/).

### 3. Build

```bash
g++ main.cpp -o chatbot -lcurl -pthread
```

### 4. Run the server

```bash
./chatbot
```

The server will start on:
👉 **[http://localhost:18080/](http://localhost:18080/)**

---

## 🔗 API Usage

### 1. Chat Endpoint

**POST** `/api/chat`

#### Request JSON

```json
{
  "session": "user123",
  "message": "Hello, AI!"
}
```

#### Response JSON

```json
{
  "reply": "Hi there! How can I help you?",
  "session": "user123"
}
```

* `session` → keeps track of conversation history
* `message` → user input
* `reply` → AI response

---

## 🎨 Frontend

You can serve a simple **`index.html`** at `/`.
Just put your HTML/JS frontend in the project root and it will be returned by the backend.

---

## 📌 Example Request (cURL)

```bash
curl -X POST http://localhost:18080/api/chat \
  -H "Content-Type: application/json" \
  -d '{"session":"demo123","message":"Tell me a joke"}'
```

---

## 🗂️ Project Structure

```
.
├── main.cpp        # C++ backend server
├── index.html      # Simple frontend (optional)
├── README.md       # Documentation
```

---

## 🚀 Future Improvements

* 🔗 Add **WebSocket** support for real-time chat
* 🔐 Add **user authentication** and DB-based session storage
* 🐳 Deploy with **Docker**
* 🎨 Enhance frontend UI with **React/Vue**

---

## 📜 License

**MIT License** – feel free to use and modify.

---
