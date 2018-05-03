let oneRow = (decoder, (rows, _)) =>
  switch (Belt_Array.length(rows)) {
  | 1 => Some(decoder(rows[0]))
  | 0 => None
  | _ => failwith("unexpected_result_count")
  };

let rows = (decoder, (rows, _)) => Belt_Array.map(rows, decoder);
