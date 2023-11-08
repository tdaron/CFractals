#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

const int WINDOW_HEIGHT = 1000;
const int WINDOW_WIDTH = 1000;
static float zoom_factor = 0.5;
static int fractal_type = 1;
static ImVec2 mandel_constant(0.0f,0.0f);
static ImVec2 julia_constant(0.258f,0.001f);                
static float precision = 250;
static float threshold = 50;


int main() {

  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT), "Fractals");
  if (!ImGui::SFML::Init(window)) {
    return 1;
  };
  
  
  sf::Clock deltaClock;
  sf::Clock clock;

  sf::RectangleShape shape(sf::Vector2f(window.getSize()));

  sf::Shader shader;
  if (!shader.loadFromFile("data/shader.frag", sf::Shader::Fragment)) {
    return -1;
  }

  sf::Vector2f mouse_position = sf::Vector2f{};
  while (window.isOpen())
  {
      //Event handling
      sf::Event event;
      while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);
          if (event.type == sf::Event::Closed) {
            window.close();
          }
          if (event.type == sf::Event::MouseMoved) {
            mouse_position = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
          }
          if (event.type == sf::Event::MouseWheelScrolled) {
            zoom_factor += event.mouseWheelScroll.delta * 0.2;
            zoom_factor = std::max(zoom_factor, 0.1f);
          }    
      }
      shader.setUniform("u_resolution", sf::Glsl::Vec2{ window.getSize() });
      shader.setUniform("u_mouse", sf::Glsl::Vec2{ mouse_position });
      shader.setUniform("u_time", clock.getElapsedTime().asSeconds());
      shader.setUniform("zoom_factor", zoom_factor);
      shader.setUniform("fractal_type", fractal_type);
      shader.setUniform("precision", precision);
      shader.setUniform("threshold", threshold);


      ImVec2* constant = &mandel_constant;
      if (fractal_type == 1) {
        constant = &julia_constant;
      }
      shader.setUniform("u_constant", sf::Glsl::Vec2{(*constant).x, (*constant).y});

      ImGui::SFML::Update(window, deltaClock.restart());
      const char* fractal_types[2] = {"Mandelbrot", "Julia"};
      ImGui::Begin("Fractals config");
      ImGui::Combo("Fractal type", &fractal_type, fractal_types, 2);
      ImGui::SliderFloat("Precision", &precision, 25.0, 1000.0);
      ImGui::SliderFloat("Threshold", &threshold, 25.0, 1000.0);
      if (fractal_type == 0) {
        ImGui::DragFloat2("Constant", (float*) &mandel_constant, 0.005f, -2.0, 2.0);
      }
      if (fractal_type == 1) {
        ImGui::DragFloat2("Constant", (float*) &julia_constant, 0.005f, -2.0, 2.0);
      }
      ImGui::End();
    
    

      //rendering part
      window.clear(sf::Color::Black);
      window.draw(shape, &shader);
      ImGui::SFML::Render(window);
      window.display();
      
    
  }
  ImGui::SFML::Shutdown();
}
  
