#include "MazeWindow.h"
#include <cmath>

MazeWindow::MazeWindow():Window (WINDOW_WIDTH, WINDOW_HEIGHT) {
SDL_SetWindowTitle(_window.get(), "Maze");
_map=std::make_shared<Map>("map.txt");

player.spawn(_map);
}
void MazeWindow::draw_minimap(Uint8 alpha ) {
	for (int row = 0; row < _map->height(); ++row) {
		for (int col = 0; col < _map->height(); ++col) {
			if (_map->is_wall(col, row))
				SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(_renderer.get(), 100, 100, 100, 255);

			SDL_Rect cell_rect { col * 50, row * 50, 50, 50 };
			SDL_RenderFillRect(_renderer.get(), &cell_rect);
		}
	}
	SDL_Point player_line[2];
	player_line[0].x = int(player.x() * 50);
	player_line[0].y = int(player.y() * 50);
	player_line[1].x = player_line[0].x + int(30 * cos(player.dir()));
	player_line[1].y = player_line[0].y + int(30 * sin(player.dir()));
	SDL_SetRenderDrawColor(_renderer.get(), 200, 200, 96, 255);
	SDL_RenderDrawLines(_renderer.get(), player_line, 2);
	SDL_Rect player_rect { int(player.x() * 50 - 5), int(player.y() * 50 - 5),
			11, 11 };
	SDL_SetRenderDrawColor(_renderer.get(), 255, 127, 127, 255);
	SDL_RenderFillRect(_renderer.get(), &player_rect);
}
void MazeWindow::cast_ray(double &rx, double &ry, double dx, double dy){
	do{
		rx+=dx;
		ry+=dy;
	} while (not _map->is_wall(rx,ry));
}

void MazeWindow::draw_col(int col, int hh)
{
	SDL_SetRenderDrawColor(_renderer.get(), 32,32,192,255);
	SDL_RenderDrawLine(_renderer.get(), col , height()/2 - hh/2, col, height()/2 + hh/2);
}

void MazeWindow::draw_col_textured (int col, int hh, Texture &tex, double tx){
	tex.draw_col(col, height()/2-hh/2,hh,tx);
}
static constexpr double EPSILON = 0.0001;

void MazeWindow::draw_view()
{
	double alpha=player.dir();
	double px=player.x();
	double py=player.y();
	double beta, gamma;
	double fov=acos(-1)/3;
	double Ds=_width/2.0/tan(fov/2.0);
	double tx,txv,txh;

	SDL_Rect sky_rect {0,0,width(), height()/2};
	SDL_Rect floor_rect{0, height()/2, width(), height()/2};
	SDL_SetRenderDrawColor(_renderer.get(), 64, 128, 192, 255);
	SDL_RenderFillRect(_renderer.get(), &sky_rect);
	SDL_SetRenderDrawColor(_renderer.get(), 64, 64, 64, 255);
	SDL_RenderFillRect(_renderer.get(), &floor_rect);

	for (int col=0; col<width(); ++col){
		double rx,ry,dx,dy;
		double Dh, Dv,Dw;
		double tpv, tph, tp;
		int h;
		int ctv,cth, ct=1;
		gamma=atan2(double(col-width()/2), Ds);
		beta=alpha+gamma;

		if(sin(beta)>EPSILON)
		{
			dy=1;
			dx=1/tan(beta);
			ry=floor(py)+EPSILON;
			rx=px-(py-ry)*dx;
			cast_ray(rx,ry,dx,dy);
			Dh=hypot(rx-px,ry-py);
			txh=rx-floor(rx);
			cth=_map->cell_type(rx, ry);
			tpv=_map->clear_transparent(rx, ry);
		} else if (sin(beta)<-EPSILON){
			dy=-1;
			dx=1/tan(-beta);
			ry=ceil(py)-EPSILON;
			rx=px-(ry-py)*dx;
			cast_ray(rx,ry,dx,dy);
			Dh=hypot(rx-px,ry-py);
			txh=rx-floor(rx);
			cth=_map->cell_type(rx, ry);
			tph=_map->clear_transparent(rx, ry);

		}else{
			Dh=INFINITY;
			txh=0.0;
			cth=1;
			tph=1;
		}

		if(cos(beta)>EPSILON)
				{
					dx=1;
					dy=tan(beta);
					rx=floor(px)+EPSILON;
					ry=py-(px-rx)*dy;
					cast_ray(rx,ry,dx,dy);
					Dv=hypot(rx-px,ry-py);
					txv=ry-floor(ry);
					ctv=_map->cell_type(rx, ry);
					tpv=_map->clear_transparent(rx, ry);
				} else if (cos(beta)<-EPSILON){
					dx=-1;
					dy=tan(-beta);
					rx=ceil(px)-EPSILON;
					ry=py-(rx-px)*dy;
					cast_ray(rx,ry,dx,dy);
					Dv=hypot(rx-px,ry-py);
					txv=ry-floor(ry);
					ctv=_map->cell_type(rx, ry);
					tph=_map->clear_transparent(rx, ry);

				}else{
					Dv=INFINITY;
					txv=0.0;
					ctv=1;
					tpv=1;
				}

		if (Dv<Dh){
			Dw=Dv;
			tx=txv;
			ct=ctv;
			tp=tpv;
		} else {
			Dw=Dh;
			tx=txh;
			ct=cth;
			tp=tph;
		}
		h=int(Ds/Dw/cos(gamma));
		switch (ct){
		case 2: draw_col_textured(col,h,_wall2,tx);break;
		case 3: draw_col_textured(col,h,_wall3,tx);break;
		default:
			draw_col_textured(col,h,_wall1,tx);break;
		}
		switch (tp){
		case 4: draw_col_textured(col,h,_wall4,tx);break;
		case 5: draw_col_textured(col,h,_wall5,tx);break;
		default:
			draw_col_textured(col,h,_wall1,tx);break;
		}
	}

}
void MazeWindow::render() {
	 draw_view();
	draw_minimap(127);
}

void MazeWindow::handle_keys(const Uint8 *keys) {
	if(keys[SDL_SCANCODE_W]) player.walk_forward();
	if(keys[SDL_SCANCODE_S]) player.walk_backward();
	if(keys[SDL_SCANCODE_D]) player.shift_right();
	if(keys[SDL_SCANCODE_A]) player.shift_left();
	if(keys[SDL_SCANCODE_E]) player.turn_right();
	if(keys[SDL_SCANCODE_Q]) player.turn_left();

}
