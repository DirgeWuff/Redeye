//
// Created by DirgeWuff on 10/1/25.
//

#ifndef APPLICATION_H
#define APPLICATION_H

class Application {
    static Application m_appInstance;
    bool m_gameRunning;

    Application();
    ~Application() = default;

    Application(Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&) = delete;
    Application&& operator=(Application&&) = delete;
public:
    void init();
    void run() const;

    static Application& getInstance() noexcept;
    void shutdown() noexcept;
};

#endif //APPLICATION_H
