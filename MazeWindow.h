#ifndef MAZEWINDOW_H_
#define MAZEWINDOW_H_
#include <memory>
#include "Window.h"
#include "Map.h"
#include "Player.h"
#include "Texture.h"
class MazeWindow final : public Window {
private:
	static constexpr int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 720;
	std:: shared_ptr<Map> _map;
	Player player;
	Texture _wall1 {_renderer, "wall1.png"};
	Texture _wall2 {_renderer, "wall2.png"};
	Texture _wall3 {_renderer, "wall3.png"};
	Texture _wall4 {_renderer, "wall4.png"};
	Texture _wall5 {_renderer, "wall5.png"};


	void draw_minimap(Uint8 alpha);
	void draw_view();
	void cast_ray(double &rx, double &ry, double dx, double dy);
	void draw_col(int col, int hh);
	void draw_col_textured(int col, int hh, Texture &tex, double tx);

public:
	MazeWindow();
	virtual ~MazeWindow() = default;
	virtual void render() override;
	virtual void handle_keys(const Uint8 *keys);
};

#endif /* MAZEWINDOW_H_ */
