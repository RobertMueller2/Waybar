#include "modules/sni/tray.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace waybar::modules::SNI {

Tray::Tray(const std::string& id, const Bar& bar, const Json::Value& config)
    : AModule(config, "tray", id),
      box_(bar.orientation, 0),
      watcher_(SNI::Watcher::getInstance()),
      host_(nb_hosts_, config, bar, std::bind(&Tray::onAdd, this, std::placeholders::_1),
            std::bind(&Tray::onRemove, this, std::placeholders::_1),
            std::bind(&Tray::update, this)) {
  box_.set_name("tray");
  event_box_.add(box_);
  if (!id.empty()) {
    box_.get_style_context()->add_class(id);
  }
  box_.get_style_context()->add_class(MODULE_CLASS);
  if (config_["spacing"].isUInt()) {
    box_.set_spacing(config_["spacing"].asUInt());
  }
  nb_hosts_ += 1;
  dp.emit();
}

void Tray::onAdd(std::unique_ptr<Item>& item) {
  if (config_["reverse-direction"].isBool() && config_["reverse-direction"].asBool()) {
    box_.pack_end(item->event_box);
  } else {
    box_.pack_start(item->event_box);
  }
  dp.emit();
}

void Tray::onRemove(std::unique_ptr<Item>& item) {
  box_.remove(item->event_box);
  dp.emit();
}

auto Tray::update() -> void {
  // Show tray only when items are available
  std::vector<Gtk::Widget*> children = box_.get_children();
  event_box_.set_visible(std::any_of(children.begin(), children.end(),
                                     [](Gtk::Widget* child) { return child->get_visible(); }));
  // Call parent update
  AModule::update();
}

}  // namespace waybar::modules::SNI
