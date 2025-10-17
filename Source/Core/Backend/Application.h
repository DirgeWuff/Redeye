//
// Created by DirgeWuff on 10/1/25.
//

#ifndef APPLICATION_H
#define APPLICATION_H

class Application {
    static Application m_appInstance;
    bool m_gameRunning;

    Application();
    Application(Application& other) = delete;
    Application(Application&& other) = delete;
    Application& operator=(Application& other) = delete;
    Application&& operator=(Application&& other) = delete;
    ~Application() = default;
public:
    void init();
    void run() const;

    static Application& getInstance() noexcept;
    void shutdown() noexcept;
};

#endif //APPLICATION_H
