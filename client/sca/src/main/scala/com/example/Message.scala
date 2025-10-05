package com.example

import io.circe.{Decoder, Encoder}
import io.circe.generic.semiauto._

case class Message(
  id: Int,
  timestamp: String,
  message: String
)

object Message {
  implicit val encoder: Encoder[Message] = deriveEncoder[Message]
  implicit val decoder: Decoder[Message] = deriveDecoder[Message]
}