import { ControlInput, ControlOutputInteger, ControlOutputString } from "./ControlReferenceInterface";
import { useDrag } from 'react-dnd'

export enum DcsBiosDraggableTypes {
    INPUT = "dcsBiosInput",
    OUTPUT_INTEGER = "dcsBiosOutputInteger",
    OUTPUT_STRING = "dcsBiosOutputString",
}

export interface DcsBiosDraggableItem {
    module: string;
    identifier: string;
    type: DcsBiosDraggableTypes;
    input?: ControlInput;
    output?: ControlOutputInteger | ControlOutputString;
}

interface DragItemProps {
    className: string;
    module: string;
    identifier: string;
    type: DcsBiosDraggableTypes;
    input?: ControlInput;
    output?: ControlOutputInteger | ControlOutputString;
    onHover: (arg: string) => void;
    onLeave: () => void;
}

export function DragItemCard(props: DragItemProps) {
    const itemDetails: DcsBiosDraggableItem = {
        module: props.module,
        identifier: props.identifier,
        type: props.type,
        input: props.input || undefined,
        output: props.output || undefined
    };

    const [{ isDragging }, dragRef] = useDrag({
        type: props.type,
        item: itemDetails,
        collect: (monitor) => ({
            isDragging: monitor.isDragging()
        })
    })

    function shareDescriptionOnHover() {
        props.onHover(props.input?.description || props.output?.description || "");
    }

    function DisplayText() {
        if (props.type === DcsBiosDraggableTypes.INPUT) {
            return "Input.SetValue";
        } else if (props.type === DcsBiosDraggableTypes.OUTPUT_INTEGER) {
            return "Output.Integer";
        } else if (props.type === DcsBiosDraggableTypes.OUTPUT_STRING) {
            return "Output.String";
        }
    }

    return (
        <div
            className={props.className}
            ref={dragRef}
            style={{ opacity: isDragging ? '0.5' : '1' }}
            onMouseEnter={shareDescriptionOnHover}
            onMouseLeave={props.onLeave}
        >
            {DisplayText()}
        </div>
    )
}
