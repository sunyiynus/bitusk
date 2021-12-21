#include "peer.hpp"


RequestPiece::RequestPiece(size_t i, size_t so, size_t len): index(i), slice_offset(so), length(len) {}



Peer& Peer::SetState(int st) {
    state = st;
    return *this;
}
