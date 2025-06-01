#pragma once

namespace hydra::frontend::native::cocoa {

class Native {
  public:
    Native();
    ~Native();

    bool ShowInputTextDialog(const std::string& header_text,
                             std::string& out_text);

  private:
    id text_input_delegate;
};

} // namespace hydra::frontend::native::cocoa
